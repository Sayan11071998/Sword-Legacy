#include "DataAssets/UI/SL_DataAsset_DataListEntryMapping.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"

TSubclassOf<USL_Widget_ListEntry_Base> USL_DataAsset_DataListEntryMapping::FindEntryWidgetClassByDataObject(
	TObjectPtr<USL_ListDataObject_Base> InDataObject) const
{
	check(InDataObject);
	
	for (UClass* DataObjectClass = InDataObject->GetClass(); DataObjectClass; DataObjectClass = DataObjectClass->GetSuperClass())
	{
		if (TSubclassOf<USL_ListDataObject_Base> ConvertedDataObjectClass = TSubclassOf<USL_ListDataObject_Base>(DataObjectClass))
		{
			if (DataObjectListEntryMap.Contains(ConvertedDataObjectClass))
			{
				return DataObjectListEntryMap.FindRef(ConvertedDataObjectClass);
			}
		}
	}
	
	return TSubclassOf<USL_Widget_ListEntry_Base>();
}