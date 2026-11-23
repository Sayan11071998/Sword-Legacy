#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_MeleeAttack_Base.h"
#include "Engine/EngineTypes.h"
#include "SL_GA_Enemy_ComboAttack.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_ComboAttack : public USL_GA_Enemy_MeleeAttack_Base
{
	GENERATED_BODY()

public:
	USL_GA_Enemy_ComboAttack();

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

	UPROPERTY(EditDefaultsOnly, Category = "Combo Attack", meta = (ClampMin = "0.0"))
	float ComboCooldown = 8.f;

private:
	UFUNCTION()
	void ClearComboCooldown();

	FTimerHandle ComboCooldownTimer;
	bool bCooldownStarted = false;
};