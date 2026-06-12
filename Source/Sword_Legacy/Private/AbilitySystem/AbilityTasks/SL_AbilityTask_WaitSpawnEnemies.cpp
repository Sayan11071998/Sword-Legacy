#include "AbilitySystem/AbilityTasks/SL_AbilityTask_WaitSpawnEnemies.h"

USL_AbilityTask_WaitSpawnEnemies* USL_AbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility,
	FGameplayTag EventTag, TSoftClassPtr<ASL_EnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn,
	const FVector& SpawnOrigin, float RandomSpawnRadius, const FRotator SpawnRotation)
{
	USL_AbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<USL_AbilityTask_WaitSpawnEnemies>(OwningAbility);
	
	Node->CachedEventTag = EventTag;
	Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
	Node->CachedNumToSpawn = NumToSpawn;
	Node->CachedSpawnOrigin = SpawnOrigin;
	Node->CachedRandomSpawnRadius = RandomSpawnRadius;
	Node->CachedSpawnRotation = SpawnRotation;
	
	return Node;
}