#include "AI/Decorators/SL_BTD_CheckCurrentHealthPercent.h"
#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SL_AttributeSet.h"

USL_BTD_CheckCurrentHealthPercent::USL_BTD_CheckCurrentHealthPercent()
{
	NodeName = TEXT("Check Current Health Percent");
	
	CheckOperation = ESL_DecoratorOperation::IsLessThanOrEqualTo;
	ThresholdToCheck = 0.5f;
}

FString USL_BTD_CheckCurrentHealthPercent::GetStaticDescription() const
{
	FString OpStr;
	
	switch (CheckOperation)
	{
	case ESL_DecoratorOperation::IsLessThan:
		OpStr = TEXT("<");
		break;
	
	case ESL_DecoratorOperation::IsLessThanOrEqualTo:
		OpStr = TEXT("<=");
		break;
		
	case ESL_DecoratorOperation::IsEqualTo:
		OpStr = TEXT("==");
		break;
		
	case ESL_DecoratorOperation::IsGreaterThanOrEqualTo:
		OpStr = TEXT(">=");
		break;
		
	case ESL_DecoratorOperation::IsGreaterThan:
		OpStr = TEXT(">");
		break;
	}
	
	return FString::Printf(TEXT("%s: Health Percent %s %.2f"), *Super::GetStaticDescription(), *OpStr, ThresholdToCheck);
}

bool USL_BTD_CheckCurrentHealthPercent::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if (!AIOwner) return false;
	
	APawn* ControlledPawn = AIOwner->GetPawn();
	if (!ControlledPawn) return false;
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn);
	if (!ASC) return false;
	
	const float CurrentHealth = ASC->GetNumericAttribute(USL_AttributeSet::GetCurrentHealthAttribute());
	const float MaxHealth = ASC->GetNumericAttribute(USL_AttributeSet::GetMaxHealthAttribute());
	
	if (MaxHealth <= 0.0f) return false;

	const float CurrentHealthPercent = CurrentHealth / MaxHealth;

	switch (CheckOperation)
	{
	case ESL_DecoratorOperation::IsLessThan:
		return CurrentHealthPercent < ThresholdToCheck;
		
	case ESL_DecoratorOperation::IsLessThanOrEqualTo:
		return CurrentHealthPercent <= ThresholdToCheck;
		
	case ESL_DecoratorOperation::IsEqualTo:
		return FMath::IsNearlyEqual(CurrentHealthPercent, ThresholdToCheck);
		
	case ESL_DecoratorOperation::IsGreaterThanOrEqualTo:
		return CurrentHealthPercent >= ThresholdToCheck;
		
	case ESL_DecoratorOperation::IsGreaterThan:
		return CurrentHealthPercent > ThresholdToCheck;
	}
	
	return false;
}