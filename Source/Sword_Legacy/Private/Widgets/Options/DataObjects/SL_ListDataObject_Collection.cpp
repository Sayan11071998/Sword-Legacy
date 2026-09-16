#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"

void USL_ListDataObject_Collection::AddChildListData(TObjectPtr<USL_ListDataObject_Base> InChildListData)
{
	// Notify the Child List Data to Init itself
	InChildListData->InitDataObject();
	
	// Set the Child List Data's parent to this
	InChildListData->SetParentData(this);
	
	ChildListDataArray.Add(InChildListData);
}

TArray<TObjectPtr<USL_ListDataObject_Base>> USL_ListDataObject_Collection::GetAllChildListData() const
{
	return ChildListDataArray;
}

bool USL_ListDataObject_Collection::HasAnyChildListData() const
{
	return !ChildListDataArray.IsEmpty();
}