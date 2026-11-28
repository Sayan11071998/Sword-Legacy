#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "Engine/EngineTypes.h"
#include "SL_GA_Enemy_SideDodge.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_SideDodge : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Enemy_SideDodge();

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

	UPROPERTY(EditDefaultsOnly, Category = "Side Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Side Dodge", meta = (ClampMin = "0.0"))
	float DodgeCooldown = 4.f;

private:
	UFUNCTION()
	void OnDodgeFinished();

	UFUNCTION()
	void ClearDodgeCooldown();

	FTimerHandle DodgeCooldownTimer;
	bool bDodgeFinished = false;
};