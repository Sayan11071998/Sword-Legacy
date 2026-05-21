#include "AI/Tasks/SL_BTT_RotateToFaceTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

USL_BTT_RotateToFaceTarget::USL_BTT_RotateToFaceTarget()
{
	NodeName = TEXT("Native Rotate To Face Target Actor");
	
	AnglePrecision = 10.f;
	RotationInterpSpeed = 5.f;
	
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
	
	INIT_TASK_NODE_NOTIFY_FLAGS();
	
	InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetToFaceKey), AActor::StaticClass());
}

void USL_BTT_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (GetBlackboardAsset())
	{
		if (UBlackboardData* BBAsset = GetBlackboardAsset())
		{
			InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
		}
	}
}

uint16 USL_BTT_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetTaskMemory);
}

FString USL_BTT_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();
	
	return FString::Printf(TEXT("Smoothly Rotates to Face %s Key Until the Angle Precision %s is Reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}

EBTNodeResult::Type USL_BTT_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);
	APawn* OwningPawn =  OwnerComp.GetAIOwner()->GetPawn();
	
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	check(Memory);
	
	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;
	
	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}
	
	if (HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::InProgress;
}

void USL_BTT_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	
	if (!Memory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	
	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			Memory->OwningPawn->GetActorLocation(),
			Memory->TargetActor->GetActorLocation()	
		);
		
		const FRotator TargetRot = FMath::RInterpTo(
			Memory->OwningPawn->GetActorRotation(),
			LookAtRot,
			DeltaSeconds,
			RotationInterpSpeed
		);
		
		Memory->OwningPawn->SetActorRotation(TargetRot);
	}
}

bool USL_BTT_RotateToFaceTarget::HasReachedAnglePrecision(TObjectPtr<APawn> QueryPawn,
	TObjectPtr<AActor> TargetActor) const
{
	const FVector OwnerForward = QueryPawn->GetActorForwardVector();
	const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal();
	
	float DotResult = FVector::DotProduct(OwnerForward, OwnerToTargetNormalized);
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);
	
	return AngleDiff <= AnglePrecision;
}