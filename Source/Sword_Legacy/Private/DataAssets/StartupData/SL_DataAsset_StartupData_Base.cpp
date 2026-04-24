#include "DataAssets/StartupData/SL_DataAsset_StartupData_Base.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"

void USL_DataAsset_StartupData_Base::GiveToAbilitySystemComponent(
	TObjectPtr<USL_AbilitySystemComponent> InASCToGive, int32 ApplyLevel)
{
	check(InASCToGive);
	
	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);
}

void USL_DataAsset_StartupData_Base::GrantAbilities(const TArray<TSubclassOf<USL_GameplayAbility>>& InAbilitiesToGive,
	TObjectPtr<USL_AbilitySystemComponent> InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty()) return;
	
	for (const TSubclassOf<USL_GameplayAbility>& Ability : InAbilitiesToGive)
	{
		if (!Ability) continue;
		
		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		
		InASCToGive->GiveAbility(AbilitySpec);
	}
}