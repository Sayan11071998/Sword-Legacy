#include "AI/Services/SL_BTS_OrientToTargetActor.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

USL_BTS_OrientToTargetActor::USL_BTS_OrientToTargetActor()
{
	NodeName = TEXT("Native Orient Rotation To Target Actor");
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
	RotationInterpSpeed = 5.f;
	Interval = 0.f;
	RandomDeviation = 0.f;
	
	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

void USL_BTS_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

FString USL_BTS_OrientToTargetActor::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();
	
	return FString::Printf(TEXT("Orient Rotation To %s Key %s"), *KeyDescription, *GetStaticServiceDescription());
}

void USL_BTS_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);
	
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	if (OwningPawn && TargetActor)
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			OwningPawn->GetActorLocation(),
			TargetActor->GetActorLocation()
		);
		
		const FRotator TargetRot = FMath::RInterpTo(
			OwningPawn->GetActorRotation(),
			LookAtRot,
			DeltaSeconds,
			RotationInterpSpeed
		);
		
		OwningPawn->SetActorRotation(TargetRot);
	}
}