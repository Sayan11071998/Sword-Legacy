#pragma once

#include "SL_PawnStructTypes.generated.h"

class USL_PlayerLinkedAnimLayer;

USTRUCT(BlueprintType)
struct FSL_PlayerWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<USL_PlayerLinkedAnimLayer> WeaponAnimLayerToLink;
};