#include "AI/Decorators/SL_BTD_ComputeSuccessChance.h"

USL_BTD_ComputeSuccessChance::USL_BTD_ComputeSuccessChance()
{
	NodeName = TEXT("Do I Want To Attack?");
}

FString USL_BTD_ComputeSuccessChance::GetStaticDescription() const
{
	return FString::Printf(TEXT("Chance: %s to %s"), *FString::SanitizeFloat(SuccessChanceMin), *FString::SanitizeFloat(SuccessChanceMax));
}

bool USL_BTD_ComputeSuccessChance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	const float RolledChance = FMath::FRandRange(SuccessChanceMin, SuccessChanceMax);
	return FMath::FRand() < RolledChance;
}