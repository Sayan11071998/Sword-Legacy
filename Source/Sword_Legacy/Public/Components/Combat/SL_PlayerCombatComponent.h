#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/SL_PawnCombatComponent.h"
#include "SL_PlayerCombatComponent.generated.h"

class ASL_PlayerWeapon;

UCLASS()
class SWORD_LEGACY_API USL_PlayerCombatComponent : public USL_PawnCombatComponent
{
	GENERATED_BODY()
	
public:
	// ~ Begin USL_PawnCombatComponent Interface
	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;
	// ~ End USL_PawnCombatComponent Interface
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	ASL_PlayerWeapon* GetPlayerCarriedWeaponByTag(FGameplayTag InWeaponTag) const;
};