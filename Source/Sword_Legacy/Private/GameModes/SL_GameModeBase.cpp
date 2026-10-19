#include "GameModes/SL_GameModeBase.h"
#include "Controllers/SL_PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "Input/CommonUIActionRouterBase.h"

void ASL_GameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (!NewPlayer || !NewPlayer->IsLocalPlayerController()) return;

	if (ULocalPlayer* LocalPlayer = NewPlayer->GetLocalPlayer())
	{
		if (UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>())
		{
			ActionRouter->SetActiveUIInputConfig(
				FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown),
				NewPlayer);
			return;
		}
	}

	NewPlayer->SetInputMode(FInputModeGameOnly());
	NewPlayer->SetShowMouseCursor(false);
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