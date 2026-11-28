#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "SL_GA_Enemy_ParryRebound.generated.h"

class UAnimMontage;
class UGameplayEffect;

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_ParryRebound : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Enemy_ParryRebound();

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

	UPROPERTY(EditDefaultsOnly, Category = "Parry Rebound")
	TObjectPtr<UAnimMontage> ReboundMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Parry Rebound")
	TSubclassOf<UGameplayEffect> HitReactEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Parry Rebound")
	float ReboundFallbackDuration = 0.2f;

private:
	UFUNCTION()
	void OnReboundFinished();

	FTimerHandle ReboundFallbackTimerHandle;
	FActiveGameplayEffectHandle AppliedHitReactEffectHandle;

public:
	FORCEINLINE TObjectPtr<UAnimMontage> GetReboundMontage() const { return ReboundMontage; }
};