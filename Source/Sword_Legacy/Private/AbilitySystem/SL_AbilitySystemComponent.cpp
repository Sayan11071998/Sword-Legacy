#include "AbilitySystem/SL_AbilitySystemComponent.h"

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