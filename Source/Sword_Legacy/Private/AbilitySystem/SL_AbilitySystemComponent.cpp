#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "Utilities/SL_GameplayTags.h"

void USL_AbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		
		if (InInputTag.MatchesTag(SL_GameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void USL_AbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(SL_GameplayTags::InputTag_MustBeHeld)) return;
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void USL_AbilitySystemComponent::GrantPlayerWeaponAbilities(
	const TArray<FSL_PlayerAbilitySet>& InDefaultWeaponAbilities,
	const TArray<FSL_PlayerSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel,
	TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
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
	
	for (const FSL_PlayerSpecialAbilitySet& AbilitySet : InSpecialWeaponAbilities)
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

bool USL_AbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate, bool bPickRandomMatchingAbility)
{
	check(AbilityTagToActivate.IsValid());
	
	TArray<FGameplayAbilitySpec*> FoundAbilitiesSpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitiesSpecs);
	
	if (!FoundAbilitiesSpecs.IsEmpty())
	{
		const int32 AbilityIndex = bPickRandomMatchingAbility
			? FMath::RandRange(0, FoundAbilitiesSpecs.Num() - 1) : 0;
		
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitiesSpecs[AbilityIndex];
		
		if (SpecToActivate && !SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	
	return false;
}