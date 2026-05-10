#pragma once

#include "CoreMinimal.h"
#include "Characters/SL_BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/SL_EnemyDeathInterface.h"
#include "SL_EnemyCharacter.generated.h"

class USL_EnemyUIComponent;
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
	
	// ~ Begin ISL_PawnCombatInterface Interface
	virtual TObjectPtr<USL_PawnCombatComponent> GetPawnCombatComponent() const override;
	// ~ End ISL_PawnCombatInterface Interface
	
	// ~ Begin ISL_EnemyDeathInterface Interface
	virtual void OnEnemyDied_Implementation(const TSoftObjectPtr<UNiagaraSystem>& InSoftNiagaraSystem) override;
	// ~ End ISL_EnemyDeathInterface Interface
	
	// ~ Begin ISL_PawnUIInterface Interface
	virtual TObjectPtr<USL_PawnUIComponent> GetPawnUIComponent() const override;
	// ~ End ISL_PawnUIInterface Interface
	
protected:
	// ~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USL_EnemyCombatComponent> EnemyCombatComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<USL_EnemyUIComponent> EnemyUIComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TObjectPtr<UCurveFloat> DissolveCurve;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	float TotalDissolveTime = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	FName LeftWeaponBoneName = FName(TEXT("weapon_l"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	FName DissolveParameterName = FName(TEXT("DissolveAmount"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	FName DissolveEdgeColorName = FName(TEXT("DissolveEdgeColor"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	FName DissolveNiagaraParticleColorName = FName(TEXT("DissolveParticleColor"));
	
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