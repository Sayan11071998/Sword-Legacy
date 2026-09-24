#include "Widgets/Options/SL_Widget_OptionsScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Widgets/Options/SL_OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"
#include "Widgets/Components/SL_TabListWidgetBase.h"
#include "Widgets/Components/SL_CommonListView.h"
#include "Utilities/SL_GameUserSettings.h"
#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "Widgets/Options/SL_Widget_OptionsDetailsView.h"

#include "SL_DebugHelper.h"

void USL_Widget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAction,
				true,
				FSimpleDelegate::CreateUObject(this, &USL_Widget_OptionsScreen::OnResetBoundActionTriggered)
			)
		);
	}
	
	RegisterUIActionBinding(
		FBindUIActionArgs(
			ICommonInputModule::GetSettings().GetDefaultBackAction(),
			true,
			FSimpleDelegate::CreateUObject(this, &USL_Widget_OptionsScreen::OnBackBoundActionTriggered)
		)	
	);
	
	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this, &USL_Widget_OptionsScreen::OnOptionsTabSelected);
	
	CommonListView_OptionsList->OnItemIsHoveredChanged().AddUObject(this, &USL_Widget_OptionsScreen::OnListViewItemHovered);
	CommonListView_OptionsList->OnItemSelectionChanged().AddUObject(this, &USL_Widget_OptionsScreen::OnListViewItemSelected);
}

void USL_Widget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	for (USL_ListDataObject_Collection* TabCollection : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollection) continue;
		
		const FName TabID = TabCollection->GetDataID();
		
		if (TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabID) != nullptr) continue;
		
		TabListWidget_OptionsTabs->RequestRegisterTab(TabID, TabCollection->GetDataDisplayName());
	}
}

void USL_Widget_OptionsScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	USL_GameUserSettings::Get()->ApplySettings(true);
}

TObjectPtr<USL_OptionsDataRegistry> USL_Widget_OptionsScreen::GetOrCreateDataRegistry()
{
	if (!CreatedOwningDataRegistry)
	{
		CreatedOwningDataRegistry = NewObject<USL_OptionsDataRegistry>();
		CreatedOwningDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
	}
	
	checkf(CreatedOwningDataRegistry, TEXT("Data Registry for Options Screen is not valid."));
	
	return CreatedOwningDataRegistry;
}

void USL_Widget_OptionsScreen::OnResetBoundActionTriggered()
{
	Debug::Print(TEXT("Reset Bound Action Triggered"));
}

void USL_Widget_OptionsScreen::OnBackBoundActionTriggered()
{
	DeactivateWidget();
}

void USL_Widget_OptionsScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem) return;
	
	USL_Widget_ListEntry_Base* HoveredEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem<USL_Widget_ListEntry_Base>(InHoveredItem);
	
	check(HoveredEntryWidget);
	
	HoveredEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);
	
	if (bWasHovered)
	{
		DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
			CastChecked<USL_ListDataObject_Base>(InHoveredItem),
			TryGetEntryWidgetClassName(InHoveredItem)
		);
	}
	else
	{
		if (USL_ListDataObject_Base* SelectedItem = CommonListView_OptionsList->GetSelectedItem<USL_ListDataObject_Base>())
		{
			DetailsView_ListEntryInfo->UpdateDetailsViewInfo(SelectedItem, TryGetEntryWidgetClassName(SelectedItem));
		}
	}
}

void USL_Widget_OptionsScreen::OnListViewItemSelected(UObject* InSelectedItem)
{
	if (!InSelectedItem) return;
	
	DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
		CastChecked<USL_ListDataObject_Base>(InSelectedItem),
		TryGetEntryWidgetClassName(InSelectedItem)	
	);
}

void USL_Widget_OptionsScreen::OnListViewListDataModified(USL_ListDataObject_Base* ModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason)
{
	if (!ModifiedData) return;
	
	if (ModifiedData->CanResetBackToDefaultValue())
	{
		ResettableDataArray.AddUnique(ModifiedData);
		
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}
	else
	{
		if (ResettableDataArray.Contains(ModifiedData))
		{
			ResettableDataArray.Remove(ModifiedData);
		}
	}
	
	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
}

FString USL_Widget_OptionsScreen::TryGetEntryWidgetClassName(TObjectPtr<UObject> InOwningListItem) const
{
	if (UUserWidget* FoundEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(InOwningListItem))
	{
		return FoundEntryWidget->GetClass()->GetName();
	}
	
	return TEXT("Entry Widget Not Valid");
}

void USL_Widget_OptionsScreen::OnOptionsTabSelected(FName TabID)
{
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();
	
	TArray<USL_ListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabID);
	
	CommonListView_OptionsList->SetListItems(FoundListSourceItems);
	CommonListView_OptionsList->RequestRefresh();
	
	if (CommonListView_OptionsList->GetNumItems() != 0)
	{
		CommonListView_OptionsList->NavigateToIndex(0);
		CommonListView_OptionsList->SetSelectedIndex(0);
	}
	
	ResettableDataArray.Empty();
	
	for (USL_ListDataObject_Base* FoundListSourceItem : FoundListSourceItems)
	{
		if (!FoundListSourceItem) continue;
		
		if (!FoundListSourceItem->OnListDataModified.IsBoundToObject(this))
		{
			FoundListSourceItem->OnListDataModified.AddUObject(this, &USL_Widget_OptionsScreen::OnListViewListDataModified);
		}
		
		if (FoundListSourceItem->CanResetBackToDefaultValue())
		{
			ResettableDataArray.AddUnique(FoundListSourceItem);
		}
	}
	
	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
	else
	{
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}
}