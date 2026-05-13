#include "Controllers/SL_AIController.h"
#include "Navigation/CrowdFollowingComponent.h"

#include "SL_DebugHelper.h"

ASL_AIController::ASL_AIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		Debug::Print(TEXT("UCrowdFollowingComponent valid!"), FColor::Green);
	}
}