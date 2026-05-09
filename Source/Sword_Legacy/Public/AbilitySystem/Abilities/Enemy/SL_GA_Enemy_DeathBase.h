#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "SL_GA_Enemy_DeathBase.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_DeathBase : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Enemy_DeathBase();
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TArray<TObjectPtr<UAnimMontage>> MontagesToPlay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	FGameplayTag DeathSoundGameplayCueTag;
	
private:
	UFUNCTION()
	void OnDeathFinished();
};