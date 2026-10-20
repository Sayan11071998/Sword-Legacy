#include "GameModes/SL_GameModeBase.h"
#include "Controllers/SL_PlayerController.h"
#include "Engine/GameInstance.h"
#include "GameFramework/WorldSettings.h"
#include "Subsystems/SL_LoadingScreenSubsystem.h"

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
}

void ASL_GameModeBase::UnbindLoadingScreen()
{
	UGameInstance* GameInstance = GetGameInstance();
	
	if (USL_LoadingScreenSubsystem* LoadingScreen = GameInstance ? GameInstance->GetSubsystem<USL_LoadingScreenSubsystem>() : nullptr)
	{
		LoadingScreen->OnGameplayReady.RemoveAll(this);
	}
}

void ASL_GameModeBase::HandlePlayerLost()
{
	if (bHasHandledPlayerLost) return;
	
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