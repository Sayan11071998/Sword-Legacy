#include "Widgets/Options/DataObjects/SL_ListDataObject_StringInteger.h"
#include "Utilities/SL_OptionsDataInteractionHelper.h"

void USL_ListDataObject_StringInteger::AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText)
{
	AddDynamicOption(LexToString(InIntegerValue), InDisplayText);
}

void USL_ListDataObject_StringInteger::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("Custom"));
	}
}

void USL_ListDataObject_StringInteger::OnEditDependencyDataModified(USL_ListDataObject_Base* ModifiedDependencyData,
	ESL_OptionsListDataModifyReason ModifyReason)
{
	if (DataDynamicGetter)
	{
		if (CurrentStringValue == DataDynamicGetter->GetValueAsString()) return;
		
		CurrentStringValue = DataDynamicGetter->GetValueAsString();
		
		if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
		{
			CurrentDisplayText = FText::FromString(TEXT("Custom"));
		}
		
		NotifyListDataModified(this, ESL_OptionsListDataModifyReason::DependencyModified);
	}
	
	Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifyReason);
}