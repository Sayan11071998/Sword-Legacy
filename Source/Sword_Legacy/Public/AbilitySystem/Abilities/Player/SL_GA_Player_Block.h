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
	// ~ End UGameplayAbility Interface

	UPROPERTY(EditDefaultsOnly, Category = "Block")
	TObjectPtr<UAnimMontage> BlockMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Successful Block")
	float KnockbackStrength = 80.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Successful Block")
	float KnockbackDuration = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Successful Block")
	FGameplayTag SuccessfulBlockEventTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Block|Successful Block")
	FGameplayTag SuccessfulBlockGameplayCueTag;

private:
	UFUNCTION()
	void OnSuccessfulBlockEventReceived(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnBlockFinished();
};