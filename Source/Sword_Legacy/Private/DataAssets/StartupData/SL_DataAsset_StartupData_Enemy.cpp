#include "DataAssets/StartupData/SL_DataAsset_StartupData_Enemy.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"

void USL_DataAsset_StartupData_Enemy::GiveToAbilitySystemComponent(TObjectPtr<USL_AbilitySystemComponent> InASCToGive,
	int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
	
	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf<USL_EnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass) continue;
			
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;
			
			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}