#include "DataAssets/StartupData/SL_DataAsset_StartupData_Player.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "PawnTypes/SL_PawnStructTypes.h"

void USL_DataAsset_StartupData_Player::GiveToAbilitySystemComponent(TObjectPtr<USL_AbilitySystemComponent> InASCToGive,
	int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
	
	for (const FSL_PlayerAbilitySet& AbilitySet : PlayerStartupAbilitySets)
	{
		if (!AbilitySet.IsValid()) continue;
		
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
		
		InASCToGive->GiveAbility(AbilitySpec);
	}
}