#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "ScalableFloat.h"
#include "SL_GA_Enemy_Projectile.generated.h"

class ASL_ProjectileBase;
class UGameplayEffect;

UCLASS()
class SWORD_LEGACY_API USL_GA_Enemy_Projectile : public USL_EnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Enemy_Projectile();
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData	
	) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Ability")
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Ability")
	TSubclassOf<ASL_ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Ability")
	TSubclassOf<UGameplayEffect> ProjectileDamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Ability")
	FScalableFloat ProjectileDamageScalableFloat;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Ability")
	FGameplayTag SpawnProjectileEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Ability")
	FName ProjectileSpawnSocketName;

private:
	UFUNCTION()
	void OnProjectileSpawnEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();
};