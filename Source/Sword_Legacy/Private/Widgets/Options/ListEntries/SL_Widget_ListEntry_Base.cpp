#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "CommonTextBlock.h"
#include "Components/ListView.h"
#include "CommonInputSubsystem.h"

void USL_Widget_ListEntry_Base::NativeOnListEntryWidgetHovered(bool bWasHovered)
{
	BP_OnListEntryWidgetHovered(bWasHovered, GetListItem() ? IsListItemSelected() : false);
}

void USL_Widget_ListEntry_Base::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	OnOwningListDataObjectSet(CastChecked<USL_ListDataObject_Base>(ListItemObject));
}

void USL_Widget_ListEntry_Base::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();
	
	NativeOnListEntryWidgetHovered(false);
}

FReply USL_Widget_ListEntry_Base::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
	
	if (CommonInputSubsystem && CommonInputSubsystem->GetCurrentInputType() != ECommonInputType::Gamepad)
	{
		if (UWidget* WidgetToFocus = BP_GetWidgetToFocusForGamepad())
		{
			if (TSharedPtr<SWidget> SlayWidgetToFocus = WidgetToFocus->GetCachedWidget())
			{
				return FReply::Handled().SetUserFocus(SlayWidgetToFocus.ToSharedRef());
			}
		}
	}
	
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void USL_Widget_ListEntry_Base::OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject)
{
	if (CommonText_SettingsDisplayName)
	{
		CommonText_SettingsDisplayName->SetText(InOwningListDataObject->GetDataDisplayName());
	}
	
	if (!InOwningListDataObject->OnListDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnListDataModified.AddUObject(this, &USL_Widget_ListEntry_Base::OnOwningListDataObjectModified);
	}
	
	if (!InOwningListDataObject->OnDependencyDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnDependencyDataModified.AddUObject(this, &USL_Widget_ListEntry_Base::OnOwningDependencyDataObjectModified);
	}
	
	OnToggleEditableState(InOwningListDataObject->IsDataCurrentlyEditable());
	
	CachedOwningDataObject = InOwningListDataObject;
}

void USL_Widget_ListEntry_Base::OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason) { }

void USL_Widget_ListEntry_Base::OnOwningDependencyDataObjectModified(
	USL_ListDataObject_Base* OwningModifiedDependencyData, ESL_OptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningDataObject)
	{
		OnToggleEditableState(CachedOwningDataObject->IsDataCurrentlyEditable());
	}
}

void USL_Widget_ListEntry_Base::OnToggleEditableState(bool bIsEditable)
{
	if (CommonText_SettingsDisplayName)
	{
		CommonText_SettingsDisplayName->SetIsEnabled(bIsEditable);
	}
}

void USL_Widget_ListEntry_Base::SelectThisEntryWidget()
{
	CastChecked<UListView>(GetOwningListView())->SetSelectedItem(GetListItem());
}