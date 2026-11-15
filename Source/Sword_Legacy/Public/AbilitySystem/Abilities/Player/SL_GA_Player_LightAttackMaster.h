#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_ComboAttack.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "SL_GA_Player_LightAttackMaster.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_LightAttackMaster : public USL_GA_Player_ComboAttack
{
	GENERATED_BODY()
	
public:
	USL_GA_Player_LightAttackMaster();
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp")
	FName LightAttackWarpTargetName = TEXT("LightAttackTarget");

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0"))
	float LightAttackWarpDistance = 350.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0"))
	float LightAttackStopDistance = 140.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|MotionWarp", meta = (ClampMin = "0", ClampMax = "180"))
	float LightAttackWarpHalfAngle = 70.f;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void WhileRageActive();
	
	virtual void ResetComboCount() override;

private:
	UFUNCTION()
	void OnMeleeHitEventReceived(FGameplayEventData Payload);

	void SetupLightAttackMotionWarp();
	
	TObjectPtr<AActor> FindLightAttackWarpTarget(const TObjectPtr<ASL_PlayerCharacter> Player) const;
	TObjectPtr<AActor> FindLockedTarget() const;
	
	bool IsEnemyInWarpRange(const class ASL_PlayerCharacter* Player, AActor* Enemy) const;
	
	int32 CurrentLightAttackComboCount = 1;
	int32 UsedComboCount = 1;
};