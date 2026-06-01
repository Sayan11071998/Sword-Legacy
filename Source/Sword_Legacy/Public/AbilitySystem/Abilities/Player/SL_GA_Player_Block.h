#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_Block.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_Block : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Player_Block();

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
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Successful Block")
	float KnockbackStrength = 80.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Successful Block")
	float KnockbackDuration = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Perfect Block")
	float PerfectBlockTimeWindow = 0.25f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Successful Block")
	FGameplayTag SuccessfulBlockEventTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Successful Block")
	FGameplayTag SuccessfulBlockGameplayCueTag;

	UFUNCTION(BlueprintPure, Category = "Block")
	FGameplayCueParameters MakeBlockGameplayCueParams() const;

private:
	UFUNCTION()
	void OnSuccessfulBlockEventReceived(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnBlockFinished();

	void RestoreTimeDilation();
	void StartResetJumpToFinisherTimer();
	void ResetJumpToFinisherState();
	
	float PlayerBlockActivatedTime = 0.f;
	bool bIsPerfectBlock = false;

	FTimerHandle TimeDilationTimerHandle;
	FTimerHandle ResetJumpToFinisherTimerHandle;
};