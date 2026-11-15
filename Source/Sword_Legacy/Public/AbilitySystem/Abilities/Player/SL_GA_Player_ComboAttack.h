#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_ComboAttack.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UWorld;

UCLASS(Abstract)
class SWORD_LEGACY_API USL_GA_Player_ComboAttack : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	void ReceiveComboInput();
	void OpenComboWindow(UAnimMontage* SourceMontage);
	void CloseComboWindow(UAnimMontage* SourceMontage);

protected:
	void PrepareComboAttack();
	void BindComboMontage(UAbilityTask_PlayMontageAndWait* MontageTask, UAnimMontage* Montage);

	virtual void ResetComboCount();

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ComboResetTime = 0.3f;

private:
	void RequestComboChain();
	void UnbindActiveMontageTask();
	void StartComboResetTimer(UWorld* World);

	UFUNCTION()
	void OnComboMontageCompleted();

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask = nullptr;

	UPROPERTY()
	TObjectPtr<UAnimMontage> PlayingMontage = nullptr;

	FTimerHandle ComboCountResetTimerHandle;

	bool bComboWindowOpen = false;
	bool bComboInputBuffered = false;
	bool bComboChainRequested = false;
	bool bMontageEndHandled = false;
};