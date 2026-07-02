#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "SL_GA_Enemy_SpawnStone_Base.generated.h"

class ASL_StoneBase;

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_SpawnStone_Base : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Enemy_SpawnStone_Base();

protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData	
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled	
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Stone")
	FScalableFloat StoneSpawnChanceScalableFloat;

private:
	void OnStoneClassLoaded(TSoftClassPtr<ASL_StoneBase> LoadedSoftClass, FVector SpawnLocation);
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Stone")
	bool bDestroyEnemyCharacterOnEnd = true;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Stone")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Stone")
	TSoftClassPtr<ASL_StoneBase> HealingStoneClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Stone")
	TSoftClassPtr<ASL_StoneBase> RageStoneClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Stone")
	float RandomSpawnRadius = 360.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Stone")
	float TraceDistance = 200.0f;
};