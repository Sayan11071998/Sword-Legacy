#include "AI/Services/SL_BTS_GetDistToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USL_BTS_GetDistToTarget::USL_BTS_GetDistToTarget()
{
	NodeName = TEXT("Native Get Distance To Target");
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USL_BTS_GetDistToTarget, TargetActorKey), AActor::StaticClass());
	DistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(USL_BTS_GetDistToTarget, DistanceToTargetKey));
}

void USL_BTS_GetDistToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (BlackboardComp && AIController)
	{
		if (AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName)))
		{
			if (APawn* ControlledPawn = AIController->GetPawn())
			{
				float Distance = ControlledPawn->GetDistanceTo(TargetActor);
				BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
			}
		}
	}
}