#include "AbilitySystem/AbilityTasks/SL_AbilityTask_WaitSpawnEnemies.h"
#include "AbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "Characters/SL_EnemyCharacter.h"
#include "NavigationSystem.h"

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
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &USL_AbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded)
		);
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<ASL_EnemyCharacter*>());
		}
		
		EndTask();
	}
}

void USL_AbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
	UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
	UWorld* World = GetWorld();
	
	if (!LoadedClass || !World)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<ASL_EnemyCharacter*>());
		}
		
		EndTask();
		return;
	}
	
	TArray<ASL_EnemyCharacter*> SpawnedEnemies;
	
	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	for (int32 i = 0; i < CachedNumToSpawn; i++)
	{
		FVector RandomLocation;
		
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
			this,
			CachedSpawnOrigin,
			RandomLocation,
			CachedRandomSpawnRadius
		);
		
		RandomLocation += FVector(0.f, 0.f, 150.f);
		
		ASL_EnemyCharacter* SpawnedEnemy = World->SpawnActor<ASL_EnemyCharacter>(LoadedClass, RandomLocation, CachedSpawnRotation, SpawnParam);
		
		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy);
		}
	}
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!SpawnedEnemies.IsEmpty())
		{
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			DidNotSpawn.Broadcast(TArray<ASL_EnemyCharacter*>());
		}
	}
	
	EndTask();
}