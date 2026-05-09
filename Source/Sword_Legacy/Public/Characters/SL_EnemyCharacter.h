#pragma once

#include "CoreMinimal.h"
#include "Characters/SL_BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/SL_EnemyDeathInterface.h"
#include "SL_EnemyCharacter.generated.h"

class USL_EnemyCombatComponent;

UCLASS()
class SWORD_LEGACY_API ASL_EnemyCharacter : public ASL_BaseCharacter, public ISL_EnemyDeathInterface
{
	GENERATED_BODY()
	
public:
	ASL_EnemyCharacter();
	
	// ~ Begin ACharacter Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// ~ End ACharacter Interface
	
	// ~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
	// ~ Begin ISL_PawnCombatInterface Interface
	virtual TObjectPtr<USL_PawnCombatComponent> GetPawnCombatComponent() const override;
	// ~ End ISL_PawnCombatInterface Interface
	
	// ~ Begin ISL_EnemyDeathInterface Interface
	virtual void OnEnemyDied_Implementation() override;
	// ~ End ISL_EnemyDeathInterface Interface
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USL_EnemyCombatComponent> EnemyCombatComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TObjectPtr<UCurveFloat> DissolveCurve;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	float TotalDissolveTime = 6.0f;
	
private:
	void InitEnemyStartupData();
	
	FTimeline DissolveTimeline;
    
	UFUNCTION()
	void HandleDissolveUpdate(float Value);
    
	UFUNCTION()
	void HandleDissolveFinished();
	
public:
	FORCEINLINE TObjectPtr<USL_EnemyCombatComponent> GetEnemyCombatComponent() const { return EnemyCombatComponent; }
};