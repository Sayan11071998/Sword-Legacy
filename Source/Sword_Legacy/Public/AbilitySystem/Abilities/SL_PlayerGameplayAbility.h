#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "SL_PlayerGameplayAbility.generated.h"

class ASL_PlayerCharacter;

UCLASS()
class SWORD_LEGACY_API USL_PlayerGameplayAbility : public USL_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Player|Ability")
	ASL_PlayerCharacter* GetPlayerCharacterFromActorInfo();
	
private:
	TWeakObjectPtr<ASL_PlayerCharacter> CachedPlayerCharacter;
};