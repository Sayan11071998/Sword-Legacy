#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_KeyRemap.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_KeyRemap.h"
#include "Widgets/Components/SL_CommonButtonBase.h"
#include "Subsystems/SL_UISubsystem.h"
#include "Utilities/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

void USL_Widget_ListEntry_KeyRemap::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CommonButton_RemapKey->OnClicked().AddUObject(this, &USL_Widget_ListEntry_KeyRemap::OnRemapKeyButtonClicked);
	CommonButton_ResetKeyBinding->OnClicked().AddUObject(this, &USL_Widget_ListEntry_KeyRemap::OnResetKeyBindingButtonClicked);
}

void USL_Widget_ListEntry_KeyRemap::OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningKeyRemapDataObject = CastChecked<USL_ListDataObject_KeyRemap>(InOwningListDataObject);
	
	CommonButton_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
}

void USL_Widget_ListEntry_KeyRemap::OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningKeyRemapDataObject)
	{
		CommonButton_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
	}
}

void USL_Widget_ListEntry_KeyRemap::OnRemapKeyButtonClicked()
{
	USL_UISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_Modal,
		USL_FunctionLibrary::GetGameSoftWidgetClassByTag(SL_GameplayTags::UI_Widget_KeyRemapScreen),
		[](EAsyncPushWidgetState PushState, USL_Widget_Activatable_Base* PushedWidget)
		{
			
		}
	);
}

void USL_Widget_ListEntry_KeyRemap::OnResetKeyBindingButtonClicked()
{
}