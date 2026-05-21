#include "AI/Services/SL_BTS_UpdateMotionWarpTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "AIController.h"
#include "Characters/SL_EnemyCharacter.h"
#include "MotionWarpingComponent.h"

USL_BTS_UpdateMotionWarpTarget::USL_BTS_UpdateMotionWarpTarget()
{
	NodeName = TEXT("Update Motion Warp Attack Target");
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	Interval = 0.3f;
	RandomDeviation = 0.1f;
	
	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

void USL_BTS_UpdateMotionWarpTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

FString USL_BTS_UpdateMotionWarpTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();
	return FString::Printf(TEXT("Update Motion Warp Target from %s Key %s"), *KeyDescription, *GetStaticServiceDescription());
}

void USL_BTS_UpdateMotionWarpTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (BlackboardComp && AIController)
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(InTargetActorKey.SelectedKeyName));
		ASL_EnemyCharacter* EnemyCharacter = Cast<ASL_EnemyCharacter>(AIController->GetPawn());
		
		if (EnemyCharacter && TargetActor)
		{
			if (UMotionWarpingComponent* MotionWarpComp = EnemyCharacter->GetMotionWarpingComponent())
			{
				MotionWarpComp->AddOrUpdateWarpTargetFromLocation(FName("AttackTarget"), TargetActor->GetActorLocation());
			}
		}
	}
}