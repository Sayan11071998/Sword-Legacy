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
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	ASL_PlayerWeapon* GetPlayerCarriedWeaponByTag(FGameplayTag InWeaponTag) const;
};