#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "SL_PlayerGameplayAbility.generated.h"

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
	
private:
	TWeakObjectPtr<ASL_PlayerCharacter> CachedPlayerCharacter;
	TWeakObjectPtr<ASL_PlayerController> CachedPlayerController;
};