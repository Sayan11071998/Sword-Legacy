#include "GameModes/SL_GameModeBase.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Controllers/SL_PlayerController.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "GameFramework/WorldSettings.h"
#include "GameplayTags/SL_GameplayTags.h"
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
		
		return;
	}

	FGameplayTag NextLevelTag;
	
	if (!USL_FunctionLibrary::GetNextProgressionLevelTag(this, NextLevelTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Level cleared, but no next level is set in Level Progression."));
		
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Level cleared. Loading %s."), *NextLevelTag.ToString());
	
	USL_FunctionLibrary::OpenLevelByTagWithLoadingScreen(this, NextLevelTag);
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