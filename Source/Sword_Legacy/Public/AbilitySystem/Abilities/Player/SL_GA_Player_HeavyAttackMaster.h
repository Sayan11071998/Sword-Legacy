#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_HeavyAttackMaster.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_HeavyAttackMaster : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Player_HeavyAttackMaster();

protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TMap<int32, TObjectPtr<UAnimMontage>> HeavyAttackMontagesMap;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag WeaponHitSoundGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ComboResetTime;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void WhileRageActive();
	
private:
	UFUNCTION()
	void OnMeleeHitEventReceived(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontageCompleted();

	void ResetHeavyAttackComboCount();
	
	int32 CurrentHeavyAttackComboCount = 1;
	int32 UsedComboCount = 1;
	float LastAttackTime = 0.f;

	FTimerHandle ComboCountResetTimerHandle;
};