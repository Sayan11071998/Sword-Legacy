#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "SL_GA_Enemy_HitReactBase.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_HitReactBase : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Enemy_HitReactBase();
	
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
	TObjectPtr<UAnimMontage> FrontHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> LeftHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> RightHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> BackHitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TSubclassOf<UGameplayEffect> HitReactEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	bool bFaceAttacker = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	FName HitParameterName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	float HitFlashIntensity = 1.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	float HitReactFallbackDuration = 0.2f;
	
private:
	UFUNCTION()
	void OnHitReactFinished();

	UAnimMontage* SelectHitReactMontage(AActor* Attacker) const;

	bool IsDirectionalHitReactMontage(const UAnimMontage* Montage) const;
	
	FTimerHandle HitReactFallbackTimerHandle;
	
	FActiveGameplayEffectHandle AppliedHitReactEffectHandle;
};