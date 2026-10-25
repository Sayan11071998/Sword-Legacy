#include "GameModes/SL_GameModeBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Controllers/SL_PlayerController.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Settings/SL_LevelDeveloperSettings.h"
#include "Subsystems/SL_LoadingScreenSubsystem.h"
#include "Utilities/SL_FunctionLibrary.h"

#if WITH_EDITOR
#include "WorldPartition/WorldPartition.h"
#include "WorldPartition/WorldPartitionHelpers.h"
#endif

void ASL_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = GetGameInstance();
	
	USL_LoadingScreenSubsystem* LoadingScreen = GameInstance ? GameInstance->GetSubsystem<USL_LoadingScreenSubsystem>() : nullptr;

	if (LoadingScreen && !LoadingScreen->IsGameplayReady())
	{
		LoadingScreen->OnGameplayReady.AddUObject(this, &ASL_GameModeBase::BeginGameplay);
		
		return;
	}

	BeginGameplay();
}

void ASL_GameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LevelClearHoldTimer);
	}

	UnbindLoadingScreen();
	
	Super::EndPlay(EndPlayReason);
}

void ASL_GameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Match input is enabled when the loading screen releases. Until then, keep the new pawn from moving.
	if (NewPlayer && NewPlayer->IsLocalPlayerController())
	{
		NewPlayer->SetIgnoreMoveInput(true);
		NewPlayer->SetIgnoreLookInput(true);
	}
}

void ASL_GameModeBase::BeginGameplay()
{
	if (bHasBegunGameplay) return;

	bHasBegunGameplay = true;
	UnbindLoadingScreen();

	if (UWorld* World = GetWorld())
	{
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->TimeDilation = 1.f;
			WorldSettings->CinematicTimeDilation = 1.f;
			WorldSettings->DemoPlayTimeDilation = 1.f;
			WorldSettings->SetTimeDilation(1.f);
		}

		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ASL_PlayerController* PlayerController = Cast<ASL_PlayerController>(Iterator->Get());
			
			if (!PlayerController || !PlayerController->IsLocalPlayerController()) continue;

			PlayerController->ApplyGameplayInputConfig();
		}
	}

	FadeLevelCamera(1.f, 0.f, false);

	if (USL_FunctionLibrary::IsCurrentLevelByTag(this, SL_GameplayTags::Game_Level_Final))
	{
		UE_LOG(LogTemp, Display, TEXT("This is the final level."));
	}

	BuildEnemyRoster();
}

void ASL_GameModeBase::UnbindLoadingScreen()
{
	UGameInstance* GameInstance = GetGameInstance();
	
	if (USL_LoadingScreenSubsystem* LoadingScreen = GameInstance ? GameInstance->GetSubsystem<USL_LoadingScreenSubsystem>() : nullptr)
	{
		LoadingScreen->OnGameplayReady.RemoveAll(this);
	}
}

void ASL_GameModeBase::BuildEnemyRoster()
{
	if (bEnemyRosterReady) return;

	PlacedEnemyCount = CountPlacedEnemies();
	bEnemyRosterReady = true;

	UE_LOG(LogTemp, Display, TEXT("Level roster has %d placed enemies."), PlacedEnemyCount);
	
	TryCompleteLevel();
}

int32 ASL_GameModeBase::CountPlacedEnemies() const
{
	UWorld* World = GetWorld();
	if (!World) return 0;

#if WITH_EDITOR
	if (UWorldPartition* WorldPartition = World->GetWorldPartition())
	{
		int32 Count = 0;
		
		FWorldPartitionHelpers::ForEachActorDescInstance<ASL_EnemyCharacter>(
			WorldPartition,
			[&Count](const FWorldPartitionActorDescInstance* ActorDescInstance)
			{
				if (!ActorDescInstance || ActorDescInstance->GetActorIsEditorOnly()) return true;

				++Count;
				
				return true;
			}
		);

		return Count;
	}
#endif

	int32 Count = 0;
	
	for (TActorIterator<ASL_EnemyCharacter> Iterator(World); Iterator; ++Iterator)
	{
		if (Iterator->HasAnyFlags(RF_WasLoaded))
		{
			++Count;
		}
	}

	return Count;
}

int32 ASL_GameModeBase::GetEnemyRosterSize() const
{
	return PlacedEnemyCount + SummonedEnemyCount;
}

void ASL_GameModeBase::RegisterSummonedEnemy(ASL_EnemyCharacter* Enemy)
{
	if (!Enemy || bHasHandledLevelCleared || bHasHandledPlayerLost) return;
	if (SummonedEnemies.Contains(Enemy)) return;

	SummonedEnemies.Add(Enemy);
	++SummonedEnemyCount;

	UE_LOG(LogTemp, Display, TEXT("Summoned enemy joined the level roster. %d remaining."), GetEnemyRosterSize() - DefeatedEnemyCount);
}

void ASL_GameModeBase::NotifyEnemyRosterDeath(ASL_EnemyCharacter* Enemy)
{
	if (!Enemy || bHasHandledLevelCleared || bHasHandledPlayerLost) return;
	if (DefeatedEnemies.Contains(Enemy)) return;

	DefeatedEnemies.Add(Enemy);
	++DefeatedEnemyCount;

	UE_LOG(LogTemp, Display, TEXT("Enemy roster: %d remaining."), GetEnemyRosterSize() - DefeatedEnemyCount);
	TryCompleteLevel();
}

bool ASL_GameModeBase::IsAnyPlayerDead() const
{
	const UWorld* World = GetWorld();
	if (!World) return false;

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		const APlayerController* PlayerController = Iterator->Get();
		if (!PlayerController) continue;

		if (APawn* ControlledPawn = PlayerController->GetPawn())
		{
			if (USL_FunctionLibrary::NativeDoesActorHaveTag(ControlledPawn, SL_GameplayTags::Shared_Status_Death))
			{
				return true;
			}
		}
	}

	return false;
}

void ASL_GameModeBase::TryCompleteLevel()
{
	if (!bEnemyRosterReady || bHasHandledLevelCleared || bHasHandledPlayerLost || IsAnyPlayerDead()) return;

	const int32 RosterSize = GetEnemyRosterSize();
	if (RosterSize <= 0 || DefeatedEnemyCount < RosterSize) return;

	HandleLevelCleared();
}

void ASL_GameModeBase::HandleLevelCleared()
{
	if (bHasHandledLevelCleared || bHasHandledPlayerLost) return;

	bHasHandledLevelCleared = true;

	if (USL_FunctionLibrary::IsCurrentLevelByTag(this, SL_GameplayTags::Game_Level_Final))
	{
		UE_LOG(LogTemp, Display, TEXT("Final level cleared."));

		if (UWorld* World = GetWorld())
		{
			for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				ASL_PlayerController* PlayerController = Cast<ASL_PlayerController>(Iterator->Get());
				
				if (!PlayerController || !PlayerController->IsLocalPlayerController()) continue;

				PlayerController->ShowGameEnding();
			}
		}

		return;
	}

	FGameplayTag NextLevelTag;
	
	if (!USL_FunctionLibrary::GetNextProgressionLevelTag(this, NextLevelTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Level cleared, but no next level is set in Level Progression."));
		
		return;
	}

	const USL_LevelDeveloperSettings* LevelSettings = GetDefault<USL_LevelDeveloperSettings>();
	const float HoldSeconds = LevelSettings ? LevelSettings->LevelClearHoldSeconds : 0.f;

	UE_LOG(LogTemp, Display, TEXT("Level cleared. Holding for %.1f seconds before fade."), HoldSeconds);

	PendingNextLevelTag = NextLevelTag;
	LockClearedLevel();

	if (HoldSeconds <= 0.f)
	{
		StartLevelTransitionFade();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			LevelClearHoldTimer,
			this,
			&ASL_GameModeBase::StartLevelTransitionFade,
			HoldSeconds,
			false
		);
	}
}

void ASL_GameModeBase::LockClearedLevel()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (!PlayerController) continue;

		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);

		APawn* Pawn = PlayerController->GetPawn();
		if (!Pawn) continue;

		Pawn->DisableInput(PlayerController);

		if (ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
			{
				Movement->StopMovementImmediately();
				Movement->DisableMovement();
			}
		}

		if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
		{
			TArray<FGameplayAbilitySpecHandle> HeldAbilities;

			for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystem->GetActivatableAbilities())
			{
				if (AbilitySpec.IsActive() && AbilitySpec.GetDynamicSpecSourceTags().HasTag(SL_GameplayTags::InputTag_MustBeHeld))
				{
					HeldAbilities.Add(AbilitySpec.Handle);
				}
			}

			for (const FGameplayAbilitySpecHandle& AbilityHandle : HeldAbilities)
			{
				AbilitySystem->CancelAbilityHandle(AbilityHandle);
			}
		}
	}
}

void ASL_GameModeBase::StartLevelTransitionFade()
{
	const USL_LevelDeveloperSettings* LevelSettings = GetDefault<USL_LevelDeveloperSettings>();
	const float FadeSeconds = LevelSettings ? LevelSettings->LevelTransitionFadeSeconds : 0.f;

	FadeLevelCamera(0.f, 1.f, true);

	if (FadeSeconds <= 0.f)
	{
		LoadNextLevelAfterClear();
		
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			LevelClearHoldTimer,
			this,
			&ASL_GameModeBase::LoadNextLevelAfterClear,
			FadeSeconds,
			false
		);
	}
}

void ASL_GameModeBase::LoadNextLevelAfterClear()
{
	if (!PendingNextLevelTag.IsValid()) return;

	USL_FunctionLibrary::OpenLevelByTagWithLoadingScreen(this, PendingNextLevelTag);
}

void ASL_GameModeBase::FadeLevelCamera(float FromAlpha, float ToAlpha, bool bHoldWhenFinished) const
{
	const USL_LevelDeveloperSettings* LevelSettings = GetDefault<USL_LevelDeveloperSettings>();
	
	const float FadeSeconds = LevelSettings ? LevelSettings->LevelTransitionFadeSeconds : 0.f;
	
	if (FadeSeconds <= 0.f) return;

	UWorld* World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (!PlayerController || !PlayerController->PlayerCameraManager) continue;

		if (FromAlpha > ToAlpha)
		{
			PlayerController->PlayerCameraManager->SetManualCameraFade(FromAlpha, FLinearColor::Black, false);
		}

		PlayerController->PlayerCameraManager->StartCameraFade(
			FromAlpha,
			ToAlpha,
			FadeSeconds,
			FLinearColor::Black,
			false,
			bHoldWhenFinished
		);
	}
}

void ASL_GameModeBase::HandlePlayerLost()
{
	if (bHasHandledPlayerLost || bHasHandledLevelCleared) return;
	
	bHasHandledPlayerLost = true;

	if (UWorld* World = GetWorld())
	{
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->SetTimeDilation(0.f);
		}
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ASL_PlayerController* PlayerController = Cast<ASL_PlayerController>(Iterator->Get());
		
		if (!PlayerController || !PlayerController->IsLocalPlayerController()) continue;

		PlayerController->ShowGameLoseScreen();
	}
}