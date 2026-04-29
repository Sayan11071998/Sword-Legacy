#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_LightAttackMaster.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_LightAttackMaster : public USL_PlayerGameplayAbility
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ComboResetTime;
	
private:
	UFUNCTION()
	void OnMontageCompleted();
	
	int32 CurrentLightAttackComboCount = 1;
	float LastAttackTime = 0.f;
};