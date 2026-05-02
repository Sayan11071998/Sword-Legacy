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
	
	// ~ Begin ACharacter Interface
	virtual void BeginPlay() override;
	// ~ End ACharacter Interface
	
	// ~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
	// ~ Begin ISL_PawnCombatInterface Interface
	virtual TObjectPtr<USL_PawnCombatComponent> GetPawnCombatComponent() const override;
	// ~ End ISL_PawnCombatInterface Interface
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USL_EnemyCombatComponent> EnemyCombatComponent;
	
private:
	void InitEnemyStartupData();
	
public:
	FORCEINLINE TObjectPtr<USL_EnemyCombatComponent> GetEnemyCombatComponent() const { return EnemyCombatComponent; }
};