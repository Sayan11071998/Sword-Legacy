#pragma once

#include "CoreMinimal.h"
#include "CommonNumericTextBlock.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Value.h"
#include "SL_ListDataObject_Scalar.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_Scalar : public USL_ListDataObject_Value
{
	GENERATED_BODY()
	
public:
	LIST_DATA_ACCESSOR(TRange<float>, DisplayValueRange);
	LIST_DATA_ACCESSOR(TRange<float>, OutputValueRange);
	LIST_DATA_ACCESSOR(float, SliderStepSize);
	LIST_DATA_ACCESSOR(ECommonNumericType, DisplayNumericType);
	LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, NumberFormattingOptions);
	
	static FCommonNumberFormattingOptions NoDecimal();
	static FCommonNumberFormattingOptions WithDecimal(int32 NumFracDigit);
	
	float GetCurrentValue() const;
	void SetCurrentValueFromSlider(float InNewValue);

private:
	// ~ Begin USL_ListDataObject_Base Interface
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	virtual  void OnEditDependencyDataModified(USL_ListDataObject_Base* ModifiedDependencyData, ESL_OptionsListDataModifyReason ModifyReason = ESL_OptionsListDataModifyReason::DirectlyModified) override;
	// ~ End USL_ListDataObject_Base Interface
	
	float StringToFloat(const FString& InString) const;
	
	TRange<float> DisplayValueRange = TRange<float>(0.f, 1.f);
	TRange<float> OutputValueRange = TRange<float>(0.f, 1.f);
	float SliderStepSize = 0.1f;
	ECommonNumericType DisplayNumericType = ECommonNumericType::Number;
	FCommonNumberFormattingOptions NumberFormattingOptions;
};