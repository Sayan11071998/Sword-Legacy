#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Scalar.h"

void USL_Widget_ListEntry_Scalar::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USL_Widget_ListEntry_Scalar::OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
}

void USL_Widget_ListEntry_Scalar::OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason) { }