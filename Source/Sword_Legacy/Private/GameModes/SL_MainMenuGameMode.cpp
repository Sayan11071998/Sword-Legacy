#include "GameModes/SL_MainMenuGameMode.h"
#include "Controllers/SL_PlayerController.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ASL_MainMenuGameMode::ASL_MainMenuGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ASL_PlayerController::StaticClass();
	bStartPlayersAsSpectators = true;
}

void ASL_MainMenuGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (!NewPlayer) return;

	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName("Default"), FoundCameras);

	if (!FoundCameras.IsEmpty() && IsValid(FoundCameras[0]))
	{
		NewPlayer->SetViewTarget(FoundCameras[0]);
	}
}