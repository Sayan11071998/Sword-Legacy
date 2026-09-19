#include "Widgets/Components/SL_CommonListView.h"
#include "Editor/WidgetCompilerLog.h"
#include "DataAssets/UI/SL_DataAsset_DataListEntryMapping.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"

UUserWidget& USL_CommonListView::OnGenerateEntryWidgetInternal(UObject* Item,
	TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (IsDesignTime())
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
	}
	
	if (TSubclassOf<USL_Widget_ListEntry_Base> FoundWidgetClass = DataListEntryMapping->FindEntryWidgetClassByDataObject(CastChecked<USL_ListDataObject_Base>(Item)))
	{
		return GenerateTypedEntry<USL_Widget_ListEntry_Base>(FoundWidgetClass, OwnerTable);
	}
	else
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
	}
}

#if WITH_EDITOR	
void USL_CommonListView::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
	
	if (!DataListEntryMapping)
	{
		CompileLog.Error(FText::FromString(
			TEXT("The variable DataListEntryMapping has no valid data asset assigned. ") +
			GetClass()->GetName() +
			TEXT(" needs a valid data asset to function properly."))
		);
	}
}
#endif