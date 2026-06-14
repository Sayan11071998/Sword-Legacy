#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "SL_GA_Enemy_SummonEnemies_Base.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_SummonEnemies_Base : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Enemy_SummonEnemies_Base();
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	TObjectPtr<UAnimMontage> MontageToPlay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	TSoftClassPtr<ASL_EnemyCharacter> SoftEnemyClassToSpawn;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	int32 NumberOfEnemiesToSpawn = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float RandomSpawnRadius = 200.0f;

private:
	UFUNCTION()
	void OnMontageFinished();
	
	UFUNCTION()
	void OnSpawnFinished(const TArray<ASL_EnemyCharacter*>& SpawnedEnemies);
};