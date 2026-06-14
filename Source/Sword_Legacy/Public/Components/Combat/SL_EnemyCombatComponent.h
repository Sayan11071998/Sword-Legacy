#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/SL_PawnCombatComponent.h"
#include "SL_EnemyCombatComponent.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_EnemyCombatComponent : public USL_PawnCombatComponent
{
	GENERATED_BODY()
	
public:
	// ~ Begin USL_PawnCombatComponent Interface
	virtual void OnHitTargetActor(AActor* HitActor) override;
	// ~ End USL_PawnCombatComponent Interface
	
protected:
	// ~ Begin USL_PawnCombatComponent Interface
	virtual void ToggleBodyCollisionBoxCollision(bool bShouldEnable, ESL_ToggleDamageType ToggleDamageType) override;
	// ~ Begin USL_PawnCombatComponent Interface
};