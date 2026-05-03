#include "AbilitySystem/GEExecCalc/SL_GEExecCalc_DamageTaken.h"
#include "AbilitySystem/SL_AttributeSet.h"
#include "Utilities/SL_GameplayTags.h"

struct FSL_DamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	
	FSL_DamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USL_AttributeSet, AttackPower, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(USL_AttributeSet, DefensePower, Target, false)
	}
};

static const FSL_DamageCapture& GetPawnDamageCapture()
{
	static FSL_DamageCapture PawnDamageCapture;
	return PawnDamageCapture;
}

USL_GEExecCalc_DamageTaken::USL_GEExecCalc_DamageTaken()
{
	RelevantAttributesToCapture.Add(GetPawnDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetPawnDamageCapture().DefensePowerDef);
}

void USL_GEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();
	
	float SourceAttackPower = 0.f;
	float TargetDefensePower = 0.f;
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetPawnDamageCapture().AttackPowerDef,
		EvaluateParameters,
		SourceAttackPower
	);
	
	float BaseDamage = 0.f;
	int32 UsedLightAttackComboCount = 0;
	int32 UsedHeavyAttackComboCount = 0;
	
	for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitude.Key.MatchesTagExact(SL_GameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
		}
		
		if (TagMagnitude.Key.MatchesTagExact(SL_GameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttackComboCount = TagMagnitude.Value;
		}
		
		if (TagMagnitude.Key.MatchesTagExact(SL_GameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCount = TagMagnitude.Value;
		}
	}
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetPawnDamageCapture().DefensePowerDef,
		EvaluateParameters,
		TargetDefensePower
	);
}