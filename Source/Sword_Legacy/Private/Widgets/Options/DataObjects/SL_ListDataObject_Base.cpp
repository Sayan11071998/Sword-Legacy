#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "Utilities/SL_GameUserSettings.h"

void USL_ListDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
}

void USL_ListDataObject_Base::AddEditCondition(const FSL_OptionsDataEditConditionDescriptor& InEditCondition)
{
	EditConditionDescArray.Add(InEditCondition);
}

bool USL_ListDataObject_Base::IsDataCurrentlyEditable()
{
	bool bIsEditable = true;
	
	if (EditConditionDescArray.IsEmpty())
	{
		return bIsEditable;
	}
	
	FString CachedDisabledRichReason;
	
	for (const FSL_OptionsDataEditConditionDescriptor& Condition : EditConditionDescArray)
	{
		if (!Condition.IsValid() || Condition.IsEditConditionMet()) continue;
		
		bIsEditable = false;
		
		CachedDisabledRichReason.Append(Condition.GetDisabledRichReason());
		
		SetDisabledRichText(FText::FromString(CachedDisabledRichReason));
		
		if (Condition.HasForcedStringValue())
		{
			const FString ForcedStringValue = Condition.GetDisabledForcedStringValue();
			
			// If the current value this data object has can be set to the forced value
			if (CanSetToForcedStringValue(ForcedStringValue))
			{
				OnSetToForcedStringValue(ForcedStringValue);
			}
		}
	}
	
	return bIsEditable;
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