#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "CommonTextBlock.h"
#include "Components/ListView.h"

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