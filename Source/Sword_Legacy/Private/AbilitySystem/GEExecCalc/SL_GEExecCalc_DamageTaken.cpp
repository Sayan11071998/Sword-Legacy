#include "AbilitySystem/GEExecCalc/SL_GEExecCalc_DamageTaken.h"
#include "AbilitySystem/SL_AttributeSet.h"

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