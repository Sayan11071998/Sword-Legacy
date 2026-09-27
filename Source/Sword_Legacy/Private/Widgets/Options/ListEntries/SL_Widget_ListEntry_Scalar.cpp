#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Scalar.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Scalar.h"

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
}

void USL_Widget_ListEntry_Scalar::OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason) { }