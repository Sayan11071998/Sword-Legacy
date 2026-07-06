#include "AI/Decorators/SL_BTD_ShouldAbortAllLogic.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Utilities/SL_GameplayTags.h"

USL_BTD_ShouldAbortAllLogic::USL_BTD_ShouldAbortAllLogic()
{
	NodeName = TEXT("Should Abort All Logic");

	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
	InDistToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InDistToTargetKey));
}

void USL_BTD_ShouldAbortAllLogic::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
		InDistToTargetKey.ResolveSelectedKey(*BBAsset);
	}
}

FString USL_BTD_ShouldAbortAllLogic::GetStaticDescription() const
{
	if (!CustomDescription.IsEmpty()) return CustomDescription;
	
	const FString TargetActorKeyName = InTargetActorKey.SelectedKeyName.ToString();
	const FString DistToTargetKeyName = InDistToTargetKey.SelectedKeyName.ToString();
	
	return FString::Printf(TEXT("Should Abort All Logic:\nTarget Actor Key: %s\nDistance Key: %s"), *TargetActorKeyName, *DistToTargetKeyName);
}

bool USL_BTD_ShouldAbortAllLogic::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return false;

	bool bIsTargetActorDead = false;
	bool bIsOwningAIDead = false;
	bool bIsDistNearlyZero = false;
	
	AActor* CachedTargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(InTargetActorKey.SelectedKeyName));
	if (CachedTargetActor)
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CachedTargetActor))
		{
			bIsTargetActorDead = TargetASC->HasMatchingGameplayTag(SL_GameplayTags::Shared_Status_Death);
		}
		
		const float DistanceToTarget = BlackboardComp->GetValueAsFloat(InDistToTargetKey.SelectedKeyName);
		bIsDistNearlyZero = FMath::IsNearlyEqual(DistanceToTarget, 0.f, 0.000001f);
	}
	
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (APawn* ControlledPawn = AIController->GetPawn())
		{
			if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn))
			{
				bIsOwningAIDead = OwnerASC->HasMatchingGameplayTag(SL_GameplayTags::Shared_Status_Death);
			}
		}
	}

	return bIsTargetActorDead || bIsOwningAIDead || bIsDistNearlyZero;
}