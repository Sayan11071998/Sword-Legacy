#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttackMaster.h"
#include "SL_GA_Player_HeavyAttack_Katana.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_HeavyAttack_Katana : public USL_GA_Player_HeavyAttackMaster
{
	GENERATED_BODY()

public:
	USL_GA_Player_HeavyAttack_Katana();

protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	// ~ End UGameplayAbility Interface

private:
	bool TryStartRageHeavyBlink(const FGameplayAbilityActorInfo* ActorInfo) const;
};