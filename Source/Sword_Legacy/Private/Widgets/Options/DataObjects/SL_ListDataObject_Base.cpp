#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "Utilities/SL_GameUserSettings.h"

void USL_ListDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
}

void USL_ListDataObject_Base::OnDataObjectInitialized() { }

void USL_ListDataObject_Base::NotifyListDataModified(TObjectPtr<USL_ListDataObject_Base> ModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason)
{
	OnListDataModified.Broadcast(ModifiedData, ModifyReason);
	
	if (bShouldApplyChangeImmediately)
	{
		USL_GameUserSettings::Get()->ApplySettings(true);
	}
}