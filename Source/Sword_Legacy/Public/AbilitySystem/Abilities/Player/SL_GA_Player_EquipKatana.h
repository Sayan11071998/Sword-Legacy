#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_EquipKatana.generated.h"

class ASL_PlayerWeapon;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_EquipKatana : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData	
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Equip")
	TObjectPtr<UAnimMontage> EquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Equip")
	FGameplayTag EquipEventTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Equip")
	FGameplayTag WeaponTagToEquip;
	
	UPROPERTY(EditDefaultsOnly, Category = "Equip")
	FName EquipSocketName;

private:
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnEquipEventReceived(FGameplayEventData Payload);

	void HandleEquipWeapon(ASL_PlayerWeapon* InWeapon);
};