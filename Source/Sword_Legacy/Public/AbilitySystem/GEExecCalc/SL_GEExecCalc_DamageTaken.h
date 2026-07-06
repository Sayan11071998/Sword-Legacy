#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SL_GEExecCalc_DamageTaken.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GEExecCalc_DamageTaken : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	USL_GEExecCalc_DamageTaken();
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float LightComboDamagePercentPerStep = 0.05f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float HeavyComboDamagePercentPerStep = 0.15f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (ClampMin = "0.01"))
	float MinDefensePower = 1.f;
	
	// ~ Begin UGameplayEffectExecutionCalculation Interface
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	// ~ End UGameplayEffectExecutionCalculation Interface
};