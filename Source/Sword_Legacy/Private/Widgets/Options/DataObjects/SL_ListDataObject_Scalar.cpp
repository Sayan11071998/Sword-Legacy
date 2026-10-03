#include "Widgets/Options/DataObjects/SL_ListDataObject_Scalar.h"
#include "Utilities/SL_OptionsDataInteractionHelper.h"

FCommonNumberFormattingOptions USL_ListDataObject_Scalar::NoDecimal()
{
	FCommonNumberFormattingOptions Options;
	
	Options.MaximumFractionalDigits = 0;
	
	return Options;
}

FCommonNumberFormattingOptions USL_ListDataObject_Scalar::WithDecimal(int32 NumFracDigit)
{
	FCommonNumberFormattingOptions Options;
	
	Options.MaximumFractionalDigits = NumFracDigit;
	
	return Options;
}

float USL_ListDataObject_Scalar::GetCurrentValue() const
{
	if (DataDynamicGetter)
	{
		return FMath::GetMappedRangeValueClamped(
			OutputValueRange,
			DisplayValueRange,
			StringToFloat(DataDynamicGetter->GetValueAsString())
		);
	}
	
	return 0.f;
}

void USL_ListDataObject_Scalar::SetCurrentValueFromSlider(float InNewValue)
{
	if (DataDynamicSetter)
	{
		const float ClampedValue = FMath::GetMappedRangeValueClamped(
			DisplayValueRange,
			OutputValueRange,
			InNewValue
		);
		
		DataDynamicSetter->SetValueFromString(LexToString(ClampedValue));
		
		NotifyListDataModified(this);
	}
}

bool USL_ListDataObject_Scalar::CanResetBackToDefaultValue() const
{
	if (HasDefaultValue() && DataDynamicGetter)
	{
		const float DefaultValue = StringToFloat(GetDefaultValueAsString());
		const float CurrentValue = StringToFloat(DataDynamicGetter->GetValueAsString());
		
		return !FMath::IsNearlyEqual(DefaultValue, CurrentValue, 0.01f);
	}
	
	return false;
}

bool USL_ListDataObject_Scalar::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(GetDefaultValueAsString());
			
			NotifyListDataModified(this, ESL_OptionsListDataModifyReason::ResetToDefault);
			
			return true;
		}
	}
	
	return false;
}

void USL_ListDataObject_Scalar::OnEditDependencyDataModified(USL_ListDataObject_Base* ModifiedDependencyData,
	ESL_OptionsListDataModifyReason ModifyReason)
{
	NotifyListDataModified(this, ESL_OptionsListDataModifyReason::DependencyModified);
	
	Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifyReason);
}

float USL_ListDataObject_Scalar::StringToFloat(const FString& InString) const
{
	float OutConvertedValue = 0.f;
	
	LexFromString(OutConvertedValue, *InString);
	
	return OutConvertedValue;
}