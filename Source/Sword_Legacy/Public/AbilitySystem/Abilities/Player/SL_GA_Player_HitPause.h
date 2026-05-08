#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_HitPause.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_HitPause : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Player_HitPause();
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit Pause")
	float HitPauseTimeDilation = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit Pause")
	float HitPauseTimeDuration = 0.02f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit Pause")
	TSubclassOf<UCameraShakeBase> HitPauseCameraShakeClass;
};