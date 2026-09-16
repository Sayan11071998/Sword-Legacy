#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"

TArray<TObjectPtr<USL_ListDataObject_Base>> USL_ListDataObject_Collection::GetAllChildListData() const
{
	return ChildListDataArray;
}

bool USL_ListDataObject_Collection::HasAnyChildListData() const
{
	return !ChildListDataArray.IsEmpty();
}