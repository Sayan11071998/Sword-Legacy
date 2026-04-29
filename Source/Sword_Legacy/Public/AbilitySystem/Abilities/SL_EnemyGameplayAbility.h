#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "SL_EnemyGameplayAbility.generated.h"

class ASL_EnemyCharacter;
class USL_EnemyCombatComponent;

UCLASS()
class SWORD_LEGACY_API USL_EnemyGameplayAbility : public USL_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Pawn|Ability")
	ASL_EnemyCharacter* GetEnemyCharacterFromActorInfo();
	
	UFUNCTION(BlueprintPure, Category = "Pawn|Ability")
	USL_EnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();
	
private:
	TWeakObjectPtr<ASL_EnemyCharacter> CachedEnemyCharacter;
};