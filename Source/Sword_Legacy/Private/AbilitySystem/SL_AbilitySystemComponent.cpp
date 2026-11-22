#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_ComboAttack.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "GameplayTags/SL_GameplayTags.h"

void USL_AbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;
		
		if (InInputTag.MatchesTag(SL_GameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else if (USL_GA_Player_ComboAttack* ComboAttack = Cast<USL_GA_Player_ComboAttack>(AbilitySpec.GetPrimaryInstance()))
		{
			if (AbilitySpec.IsActive())
			{
				ComboAttack->ReceiveComboInput(AbilitySpec.Handle);
			}
			else
			{
				USL_GA_Player_ComboAttack* ActiveComboAttack = nullptr;

				for (const FGameplayAbilitySpec& OtherSpec : GetActivatableAbilities())
				{
					USL_GA_Player_ComboAttack* OtherComboAttack = Cast<USL_GA_Player_ComboAttack>(OtherSpec.GetPrimaryInstance());
					if (OtherComboAttack && OtherSpec.IsActive())
					{
						ActiveComboAttack = OtherComboAttack;
						break;
					}
				}

				if (ActiveComboAttack)
				{
					ActiveComboAttack->ReceiveComboInput(AbilitySpec.Handle);
				}
				else
				{
					TryActivateAbility(AbilitySpec.Handle);
				}
			}
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
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void USL_AbilitySystemComponent::SetGrantedAbilityLevels(int32 NewLevel)
{
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		AbilityHandles.Add(AbilitySpec.Handle);
	}

	for (const FGameplayAbilitySpecHandle& AbilityHandle : AbilityHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilityHandle))
		{
			AbilitySpec->Level = NewLevel;
			MarkAbilitySpecDirty(*AbilitySpec);
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
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
	
	for (const FSL_PlayerSpecialAbilitySet& AbilitySet : InSpecialWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		
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
		
		if (SpecToActivate)
		{
			if (SpecToActivate->IsActive()) return true;

			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	
	return false;
}