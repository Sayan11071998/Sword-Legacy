#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SL_AbilityTask_WaitSpawnEnemies.generated.h"

class ASL_EnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate, const TArray<ASL_EnemyCharacter*>&, SpawnedEnemies);

UCLASS()
class SWORD_LEGACY_API USL_AbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Pawn|AbilityTasks", meta = (DisplayName = "Wait Gameplay Event And Spawn Enemies", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", NumToSpawn = "1", RandomSpawnRadius = "200"))
	static USL_AbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(
		UGameplayAbility* OwningAbility,
		FGameplayTag EventTag,
		TSoftClassPtr<ASL_EnemyCharacter> SoftEnemyClassToSpawn,
		int32 NumToSpawn,
		const FVector& SpawnOrigin,
		float RandomSpawnRadius,
		const FRotator SpawnRotation
	);
	
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;
	
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;
	
private:
	FGameplayTag CachedEventTag;
	TSoftClassPtr<ASL_EnemyCharacter> CachedSoftEnemyClassToSpawn;
	int32 CachedNumToSpawn;
	FVector CachedSpawnOrigin;
	float CachedRandomSpawnRadius;
	FRotator CachedSpawnRotation;
};