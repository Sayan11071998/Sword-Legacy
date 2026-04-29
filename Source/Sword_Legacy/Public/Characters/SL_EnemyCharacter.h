#pragma once

#include "CoreMinimal.h"
#include "Characters/SL_BaseCharacter.h"
#include "SL_EnemyCharacter.generated.h"

class USL_EnemyCombatComponent;

UCLASS()
class SWORD_LEGACY_API ASL_EnemyCharacter : public ASL_BaseCharacter
{
	GENERATED_BODY()
	
public:
	ASL_EnemyCharacter();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USL_EnemyCombatComponent> EnemyCombatComponent;
	
public:
	FORCEINLINE TObjectPtr<USL_EnemyCombatComponent> GetEnemyCombatComponent() const { return EnemyCombatComponent; }
};