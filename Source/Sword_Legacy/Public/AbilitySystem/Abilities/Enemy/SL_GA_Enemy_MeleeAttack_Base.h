#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "SL_GA_Enemy_MeleeAttack_Base.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_MeleeAttack_Base : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Enemy_MeleeAttack_Base();
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData	
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack")
	TObjectPtr<UAnimMontage> MontageToPlay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack")
	FScalableFloat InDamageScalableFloat;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack")
	FGameplayTag WeaponHitSoundGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack")
	float UnblockableAttackWarningSpawnOffset;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack")
	float UnblockableWarningDelay = 0.2f;
	
	UFUNCTION(BlueprintCallable, Category = "Melee Attack")
	void HandleApplyDamage(FGameplayEventData Payload);
	
private:
	UFUNCTION()
	void PlayMeleeAttackMontage();

	UFUNCTION()
	void OnMeleeAttackFinished();
};