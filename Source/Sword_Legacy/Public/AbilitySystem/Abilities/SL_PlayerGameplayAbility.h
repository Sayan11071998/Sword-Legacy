#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "SL_PlayerGameplayAbility.generated.h"

class USL_PlayerUIComponent;
class USL_PlayerCombatComponent;
class ASL_PlayerController;
class ASL_PlayerCharacter;

UCLASS()
class SWORD_LEGACY_API USL_PlayerGameplayAbility : public USL_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Player|Ability")
	ASL_PlayerCharacter* GetPlayerCharacterFromActorInfo();
	
	UFUNCTION(BlueprintPure, Category = "Player|Ability")
	ASL_PlayerController* GetPlayerControllerFromActorInfo();
	
	UFUNCTION(BlueprintPure, Category = "Player|Ability")
	USL_PlayerCombatComponent* GetPlayerCombatComponentFromActorInfo();
	
	UFUNCTION(BlueprintPure, Category = "Player|Ability")
	USL_PlayerUIComponent* GetPlayerUIComponentFromActorInfo();
	
	UFUNCTION(BlueprintPure, Category = "Player|Ability")
	FGameplayEffectSpecHandle MakePlayerDamageEffectSpecHandle(
		TSubclassOf<UGameplayEffect> EffectClass,
		float InWeaponBaseDamage,
		FGameplayTag InCurrentAttackTypeTag,
		int32 InUsedComboCount	
	);
	
	UFUNCTION(BlueprintCallable, Category = "Player|Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime);

	void WarnEnemyOfIncomingAttack(AActor* Enemy);

protected:
	// ~ Begin UGameplayAbility Interface
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
	// ~ End UGameplayAbility Interface

	void SetupMeleeMotionWarp(FName WarpTargetName, float WarpDistance, float StopDistance, float HalfAngle);

private:
	void ClearIncomingAttackWarnings();

	TObjectPtr<AActor> FindMeleeWarpTarget(const ASL_PlayerCharacter* Player, float WarpDistance, float HalfAngle) const;
	TObjectPtr<AActor> FindLockedWarpTarget() const;
	bool IsEnemyInMeleeWarpRange(const ASL_PlayerCharacter* Player, AActor* Enemy, float WarpDistance) const;

	TArray<TWeakObjectPtr<AActor>> WarnedEnemies;

	TWeakObjectPtr<ASL_PlayerCharacter> CachedPlayerCharacter;
	TWeakObjectPtr<ASL_PlayerController> CachedPlayerController;
};