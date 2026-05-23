#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"

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
	const TArray<FSL_PlayerAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilities.IsEmpty()) return;
	
	for (const FSL_PlayerAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
		
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void USL_AbilitySystemComponent::RemoveGrantedPlayerWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty()) return;
	
	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	
	InSpecHandlesToRemove.Empty();
}

bool USL_AbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());
	
	TArray<FGameplayAbilitySpec*> FoundAbilitiesSpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitiesSpecs);
	
	if (!FoundAbilitiesSpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitiesSpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitiesSpecs[RandomAbilityIndex];
		
		check(SpecToActivate);
		
		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	
	return false;
}