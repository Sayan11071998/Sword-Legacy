#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "SL_GA_Enemy_UnblockableHitReact.generated.h"

class UAnimMontage;
class UGameplayEffect;

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_UnblockableHitReact : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Enemy_UnblockableHitReact();

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

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> UnblockableHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TSubclassOf<UGameplayEffect> HitReactEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	FName HitParameterName;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	float HitFlashIntensity = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	float HitReactFallbackDuration = 0.2f;

private:
	UFUNCTION()
	void OnHitReactFinished();

	FTimerHandle HitReactFallbackTimerHandle;
	FActiveGameplayEffectHandle AppliedHitReactEffectHandle;
};