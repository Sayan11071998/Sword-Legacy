#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_ComboAttack.h"
#include "SL_GA_Player_HeavyAttackMaster.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_HeavyAttackMaster : public USL_GA_Player_ComboAttack
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
	TSubclassOf<UGameplayEffect> GainRageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag WeaponHitSoundGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp")
	FName HeavyAttackWarpTargetName = TEXT("HeavyAttackTarget");

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0"))
	float HeavyAttackWarpDistance = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0"))
	float HeavyAttackStopDistance = 130.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0", ClampMax = "180"))
	float HeavyAttackWarpHalfAngle = 80.f;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void WhileRageActive();
	
private:
	UFUNCTION()
	void OnMeleeHitEventReceived(FGameplayEventData Payload);

	int32 UsedComboCount = 1;
	float LastAttackTime = 0.f;
};