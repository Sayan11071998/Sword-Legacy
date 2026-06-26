#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_HeavyWeaponAbility_Katana.generated.h"

class UAnimMontage;
class UGameplayEffect;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_HeavyWeaponAbility_Katana : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Player_HeavyWeaponAbility_Katana();

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
	TObjectPtr<UAnimMontage> SpecialHeavyMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag AOEEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float WeaponDamageLevelMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 UsedComboCount;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FVector BoxTraceHalfSize;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BoxTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag AbilityInputTag;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag AbilityCooldownTag;

private:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnAOEEventReceived(FGameplayEventData Payload);
};