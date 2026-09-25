#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "CommonTextBlock.h"
#include "Components/ListView.h"
#include "CommonInputSubsystem.h"

void USL_Widget_ListEntry_Base::NativeOnListEntryWidgetHovered(bool bWasHovered)
{
	BP_OnListEntryWidgetHovered(bWasHovered, IsListItemSelected());
}

void USL_Widget_ListEntry_Base::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	SetVisibility(ESlateVisibility::Visible);
	
	OnOwningListDataObjectSet(CastChecked<USL_ListDataObject_Base>(ListItemObject));
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
}

void USL_Widget_ListEntry_Base::OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason) { }

void USL_Widget_ListEntry_Base::SelectThisEntryWidget()
{
	CastChecked<UListView>(GetOwningListView())->SetSelectedItem(GetListItem());
}