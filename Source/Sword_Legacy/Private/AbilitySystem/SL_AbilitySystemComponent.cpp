#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"

void USL_AbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		
		TryActivateAbility(AbilitySpec.Handle);
	}
}

void USL_AbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}

void USL_AbilitySystemComponent::GrantPlayerWeaponAbilities(
	const TArray<FSL_PlayerAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel)
{
	if (InDefaultWeaponAbilities.IsEmpty()) return;
	
	for (const FSL_PlayerAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
		
		GiveAbility(AbilitySpec);
	}
}