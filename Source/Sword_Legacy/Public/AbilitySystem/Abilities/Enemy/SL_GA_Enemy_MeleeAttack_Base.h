#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "SL_GA_Enemy_MeleeAttack_Base.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_MeleeAttack_Base : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Enemy_MeleeAttack_Base();
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData	
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack")
	TObjectPtr<UAnimMontage> MontageToPlay;
	
private:
	UFUNCTION()
	void OnMeleeAttackFinished();
};