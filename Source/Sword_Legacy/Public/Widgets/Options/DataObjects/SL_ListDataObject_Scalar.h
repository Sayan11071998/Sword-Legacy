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

private:
	TRange<float> DisplayValueRange = TRange<float>(0.f, 1.f);
	TRange<float> OutputValueRange = TRange<float>(0.f, 1.f);
	float SliderStepSize = 0.1f;
	ECommonNumericType DisplayNumericType = ECommonNumericType::Number;
	FCommonNumberFormattingOptions NumberFormattingOptions;
};