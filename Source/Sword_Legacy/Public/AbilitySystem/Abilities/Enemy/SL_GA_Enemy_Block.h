#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "SL_GA_Enemy_Block.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_Block : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Enemy_Block();

protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
	// ~ End UGameplayAbility Interface

	UPROPERTY(EditDefaultsOnly, Category = "Block")
	TObjectPtr<UAnimMontage> BlockMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Block")
	FName HoldSectionName = TEXT("Hold");

	UPROPERTY(EditDefaultsOnly, Category = "Block", meta = (ClampMin = "0.0"))
	float BlockHoldDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Block")
	FGameplayTag SuccessfulBlockCueTag;

private:
	void PlayBlockMontage();
	void LoopHoldSection();

	UFUNCTION()
	void OnBlockMontageCompleted();

	UFUNCTION()
	void OnBlockMontageCancelled();

	UFUNCTION()
	void OnSuccessfulBlock(FGameplayEventData Payload);

	UFUNCTION()
	void OnBlockHoldFinished();

	void FinishBlock();

	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	FTimerHandle BlockHoldTimer;
	bool bBlockFinished = false;
	bool bRestartingMontage = false;
};