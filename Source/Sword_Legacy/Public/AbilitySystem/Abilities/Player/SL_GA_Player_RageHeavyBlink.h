#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "Engine/EngineTypes.h"
#include "SL_GA_Player_RageHeavyBlink.generated.h"

class UAnimMontage;
class UGameplayEffect;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_RageHeavyBlink : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()

public:
	USL_GA_Player_RageHeavyBlink();

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

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink")
	TObjectPtr<UAnimMontage> BlinkPassMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink", meta = (ClampMin = "1"))
	int32 BlinkPassCount = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink", meta = (ClampMin = "0"))
	float BlinkPassDistance = 180.f;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink", meta = (ClampMin = "0"))
	float BlinkVanishTime = 0.12f;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink", meta = (ClampMin = "0.01"))
	float BlinkTravelTime = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink", meta = (ClampMin = "0"))
	float BlinkSearchDistance = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink")
	FGameplayTag WeaponHitSoundGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink")
	float RageDamageMultiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Rage Heavy Blink", meta = (ClampMin = "1"))
	int32 RageComboCount = 1;

private:
	void BeginBlinkChain();
	void BeginBlinkPass();
	void TravelBlinkPass();
	void FinishBlinkChain();
	void SetBlinkHidden(bool bHidden);
	void RestoreBlinkBody();
	void ApplyBlinkDamage(AActor* Target);
	void ResolveHeavyAttackDamage();
	
	TObjectPtr<AActor> FindBlinkTarget();
	
	bool IsLivingEnemy(AActor* Enemy) const;

	FTimerHandle BlinkStepTimer;
	FTimerHandle BlinkTravelTimer;
	
	TWeakObjectPtr<AActor> BlinkTarget;
	
	FVector BlinkPassStart = FVector::ZeroVector;
	FVector BlinkPassEnd = FVector::ZeroVector;
	
	float BlinkTravelAlpha = 0.f;
	
	int32 BlinkPassIndex = 0;
	
	bool bBlinkDamageApplied = false;
	bool bBlinkBodyAltered = false;
	
	ECollisionEnabled::Type SavedCapsuleCollision = ECollisionEnabled::QueryAndPhysics;
	
	TSubclassOf<UGameplayEffect> ResolvedDamageEffectClass;
	
	FGameplayTag ResolvedHitSoundCue;
};