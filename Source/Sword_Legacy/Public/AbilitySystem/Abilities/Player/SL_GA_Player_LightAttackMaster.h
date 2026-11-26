#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_ComboAttack.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "SL_GA_Player_LightAttackMaster.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_LightAttackMaster : public USL_GA_Player_ComboAttack
{
	GENERATED_BODY()
	
public:
	USL_GA_Player_LightAttackMaster();
	
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
	TMap<int32, TObjectPtr<UAnimMontage>> LightAttackMontagesMap;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Parry")
	TObjectPtr<UAnimMontage> ParryCounterMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Parry", meta = (ClampMin = "1"))
	int32 ParryCounterDamageCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> GainRageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag WeaponHitSoundGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp")
	FName LightAttackWarpTargetName = TEXT("LightAttackTarget");

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0"))
	float LightAttackWarpDistance = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0"))
	float LightAttackStopDistance = 130.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0", ClampMax = "180"))
	float LightAttackWarpHalfAngle = 80.f;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void WhileRageActive();

private:
	UFUNCTION()
	void OnMeleeHitEventReceived(FGameplayEventData Payload);

	int32 UsedComboCount = 1;
};