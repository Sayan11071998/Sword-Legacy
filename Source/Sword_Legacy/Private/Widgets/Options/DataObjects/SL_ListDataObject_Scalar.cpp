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

float USL_ListDataObject_Scalar::StringToFloat(const FString& InString) const
{
	float OutConvertedValue = 0.f;
	
	LexFromString(OutConvertedValue, *InString);
	
	return OutConvertedValue;
}