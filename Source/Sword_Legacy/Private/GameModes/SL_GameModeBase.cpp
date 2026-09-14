#include "GameModes/SL_GameModeBase.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
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