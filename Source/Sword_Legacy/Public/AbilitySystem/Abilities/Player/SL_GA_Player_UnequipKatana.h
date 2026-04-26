#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_UnequipKatana.generated.h"

class ASL_PlayerWeapon;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_UnequipKatana : public USL_PlayerGameplayAbility
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Unequip")
	TObjectPtr<UAnimMontage> UnequipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Unequip")
	FGameplayTag UnequipEventTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Unequip")
	FGameplayTag WeaponTagToUnequip;
	
	UPROPERTY(EditDefaultsOnly, Category = "Unequip")
	FName UnequipSocketName;
	
private:
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnUnequipEventReceived(FGameplayEventData Payload);
	
	void HandleUnequipWeapon(ASL_PlayerWeapon* InWeapon);
};