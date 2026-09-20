#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_String.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "Widgets/Components/SL_CommonRotator.h"

void USL_Widget_ListEntry_String::OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningStringDataObject = CastChecked<USL_ListDataObject_String>(InOwningListDataObject);
	
	CommonRotator_AvailableOptions->PopulateTextLabels(CachedOwningStringDataObject->GetAvailableOptionsTextArray());
	CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
}