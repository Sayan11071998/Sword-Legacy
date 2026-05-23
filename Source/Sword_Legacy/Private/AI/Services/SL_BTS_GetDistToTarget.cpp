#include "AI/Services/SL_BTS_GetDistToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "AIController.h"

USL_BTS_GetDistToTarget::USL_BTS_GetDistToTarget()
{
	NodeName = TEXT("Native Get Distance To Target");
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
	Interval = 0.2f;
	RandomDeviation = 0.f;
	
	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
	DistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, DistanceToTargetKey));
}

void USL_BTS_GetDistToTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
		DistanceToTargetKey.ResolveSelectedKey(*BBAsset);
	}
}

FString USL_BTS_GetDistToTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();
	
	return FString::Printf(TEXT("Get Distance To %s Key %s"), *KeyDescription, *GetStaticServiceDescription());
}

void USL_BTS_GetDistToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (BlackboardComp && AIController)
	{
		if (AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(InTargetActorKey.SelectedKeyName)))
		{
			if (APawn* ControlledPawn = AIController->GetPawn())
			{
				float Distance = ControlledPawn->GetDistanceTo(TargetActor);
				BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
			}
		}
	}
}
