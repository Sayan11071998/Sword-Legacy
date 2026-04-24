#include "DataAssets/StartupData/SL_DataAsset_StartupData_Base.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"

void USL_DataAsset_StartupData_Base::GiveToAbilitySystemComponent(
	TObjectPtr<USL_AbilitySystemComponent> InCharacterASCToGive, int32 ApplyLevel)
{
	check(InCharacterASCToGive);
	
	GrantAbilities(ActivateOnGivenAbilities, InCharacterASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InCharacterASCToGive, ApplyLevel);
}

void USL_DataAsset_StartupData_Base::GrantAbilities(const TArray<TSubclassOf<USL_GameplayAbility>>& InAbilitiesToGive,
	TObjectPtr<USL_AbilitySystemComponent> InCharacterASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty()) return;
	
	for (const TSubclassOf<USL_GameplayAbility>& Ability : InAbilitiesToGive)
	{
		if (!Ability) continue;
		
		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InCharacterASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		
		InCharacterASCToGive->GiveAbility(AbilitySpec);
	}
}