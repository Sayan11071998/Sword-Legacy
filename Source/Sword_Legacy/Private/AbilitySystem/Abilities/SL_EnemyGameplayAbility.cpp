#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "Characters/SL_EnemyCharacter.h"

ASL_EnemyCharacter* USL_EnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	if (!CachedEnemyCharacter.IsValid())
	{
		CachedEnemyCharacter = Cast<ASL_EnemyCharacter>(CurrentActorInfo->AvatarActor);
	}
	
	return CachedEnemyCharacter.IsValid() ? CachedEnemyCharacter.Get() : nullptr;
}

USL_EnemyCombatComponent* USL_EnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}