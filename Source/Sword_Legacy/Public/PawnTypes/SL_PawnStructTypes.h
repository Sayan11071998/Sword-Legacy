#pragma once

#include "GameplayTagContainer.h"
#include "SL_PawnStructTypes.generated.h"

class USL_GameplayAbility;
class USL_PlayerLinkedAnimLayer;

USTRUCT(BlueprintType)
struct FSL_PlayerAbilitySet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<USL_GameplayAbility> AbilityToGrant;
	
	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FSL_PlayerWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<USL_PlayerLinkedAnimLayer> WeaponAnimLayerToLink;
};