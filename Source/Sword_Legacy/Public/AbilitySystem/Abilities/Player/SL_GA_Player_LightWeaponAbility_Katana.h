#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_LightWeaponAbility_Katana.generated.h"

class UAnimMontage;
class UGameplayEffect;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_LightWeaponAbility_Katana : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Player_LightWeaponAbility_Katana();

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
	TObjectPtr<UAnimMontage> SpecialLightMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag MeleeHitEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag WeaponHitSoundGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float WeaponDamageLevelMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 LevelComboCount;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag AbilityInputTag;

private:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMeleeHitEventReceived(FGameplayEventData Payload);
};