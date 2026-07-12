#include "Controllers/SL_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

ASL_PlayerController::ASL_PlayerController()
{
	PlayerTeamID = FGenericTeamId(0);
}

FGenericTeamId ASL_PlayerController::GetGenericTeamId() const
{
	return PlayerTeamID;
}

void ASL_PlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
	TArray<AActor*> FoundCameras;
	
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName("Default"), FoundCameras);
	
	if (!FoundCameras.IsEmpty())
	{
		SetViewTarget(FoundCameras[0]);
	}
}