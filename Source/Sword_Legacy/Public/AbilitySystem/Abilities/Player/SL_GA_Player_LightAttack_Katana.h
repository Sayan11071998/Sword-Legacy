#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_LightAttackMaster.h"
#include "SL_GA_Player_LightAttack_Katana.generated.h"

class ASL_ProjectileBase;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_LightAttack_Katana : public USL_GA_Player_LightAttackMaster
{
	GENERATED_BODY()

public:
	USL_GA_Player_LightAttack_Katana();

protected:
	virtual void WhileRageActive_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	TSubclassOf<ASL_ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	TSubclassOf<UGameplayEffect> ProjectileDamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	FName ProjectileSpawnSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageDamageMultiplier;

private:
	UFUNCTION()
	void OnSpawnProjectileEventReceived(FGameplayEventData Payload);
};