#include "AbilitySystem/AbilityTasks/SL_AbilityTask_WaitSpawnEnemies.h"
#include "AbilitySystemComponent.h"

#include "SL_DebugHelper.h"

void USL_AbilityTask_WaitSpawnEnemies::Activate()
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	
	DelegateHandle = Delegate.AddUObject(this, &USL_AbilityTask_WaitSpawnEnemies::OnGameplayEventReceived);
}

void USL_AbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	
	Delegate.Remove(DelegateHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

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

void USL_AbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
	Debug::Print(TEXT("Gameplay Event Received"));
	
	EndTask();
}