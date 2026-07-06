#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttackMaster.h"
#include "SL_GA_Player_HeavyAttack_Katana.generated.h"

class ASL_ProjectileBase;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_HeavyAttack_Katana : public USL_GA_Player_HeavyAttackMaster
{
	GENERATED_BODY()

public:
	USL_GA_Player_HeavyAttack_Katana();

protected:
	// ~ Begin USL_GA_Player_HeavyAttackMaster Interface
	virtual void WhileRageActive_Implementation() override;
	// ~ End USL_GA_Player_HeavyAttackMaster Interface

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	TSubclassOf<ASL_ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	TSubclassOf<UGameplayEffect> ProjectileDamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	FName ProjectileSpawnSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageDamageMultiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	int32 RageComboCount = 1;

private:
	UFUNCTION()
	void OnSpawnProjectileEventReceived(FGameplayEventData Payload);
};