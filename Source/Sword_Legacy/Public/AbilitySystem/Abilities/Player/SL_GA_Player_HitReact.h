#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_HitReact.generated.h"

class UNiagaraSystem;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_HitReact : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Player_HitReact();

protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData	
	) override;
	// ~ End UGameplayAbility Interface

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> FrontHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> LeftHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> RightHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> BackHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UNiagaraSystem> BloodSplashFX;

private:
	UFUNCTION()
	void OnHitReactFinished();
};