#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_Roll.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_Roll : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Player_Roll();
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	TObjectPtr<UAnimMontage> RollMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	
private:
	void ComputeRollDirectionAndDistance();
	
	UFUNCTION()
	void OnDelayFinished();
	
	UFUNCTION()
	void OnMontageCompleted();
};