#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "SL_GA_Player_LightAttackMaster.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UWorld;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_LightAttackMaster : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Player_LightAttackMaster();

	void ReceiveComboInput();
	void OpenComboWindow(UAnimMontage* SourceMontage);
	void CloseComboWindow(UAnimMontage* SourceMontage);
	
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
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> GainRageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag WeaponHitSoundGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ComboResetTime = 0.3f;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void WhileRageActive();
	
private:
	UFUNCTION()
	void OnMeleeHitEventReceived(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontageCompleted();

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask = nullptr;

	UPROPERTY()
	TObjectPtr<UAnimMontage> PlayingMontage = nullptr;

	void ResetLightAttackComboCount();
	void RequestComboChain();
	void UnbindActiveMontageTask();
	void StartComboResetTimer(UWorld* World);
	
	int32 CurrentLightAttackComboCount = 1;
	int32 UsedComboCount = 1;
	FTimerHandle ComboCountResetTimerHandle;

	bool bComboWindowOpen = false;
	bool bComboInputBuffered = false;
	bool bComboChainRequested = false;
	bool bMontageEndHandled = false;
};