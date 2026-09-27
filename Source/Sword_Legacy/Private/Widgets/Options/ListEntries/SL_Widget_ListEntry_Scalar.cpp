#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Scalar.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Scalar.h"
#include "AnalogSlider.h"

void USL_Widget_ListEntry_Scalar::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USL_Widget_ListEntry_Scalar::OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningScalarDataObject = CastChecked<USL_ListDataObject_Scalar>(InOwningListDataObject);
	
	CommonNumeric_SettingValue->SetNumericType(CachedOwningScalarDataObject->GetDisplayNumericType());
	CommonNumeric_SettingValue->FormattingSpecification = CachedOwningScalarDataObject->GetNumberFormattingOptions();
	CommonNumeric_SettingValue->SetCurrentValue(CachedOwningScalarDataObject->GetCurrentValue());
	
	AnalogSlider_SettingSlider->SetMinValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetLowerBoundValue());
	AnalogSlider_SettingSlider->SetMaxValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetUpperBoundValue());
	AnalogSlider_SettingSlider->SetStepSize(CachedOwningScalarDataObject->GetSliderStepSize());
	AnalogSlider_SettingSlider->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
}

void USL_Widget_ListEntry_Scalar::OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason) { }