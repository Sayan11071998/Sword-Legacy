#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_KeyRemap.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_KeyRemap.h"
#include "Widgets/Components/SL_CommonButtonBase.h"
#include "Subsystems/SL_UISubsystem.h"
#include "Utilities/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Widgets/Options/SL_Widget_KeyRemapScreen.h"

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
	SelectThisEntryWidget();
	
	USL_UISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_Modal,
		USL_FunctionLibrary::GetGameSoftWidgetClassByTag(SL_GameplayTags::UI_Widget_KeyRemapScreen),
		[this](EAsyncPushWidgetState PushState, USL_Widget_Activatable_Base* PushedWidget)
		{
			if (PushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				USL_Widget_KeyRemapScreen* CreatedKeyRemapScreen = CastChecked<USL_Widget_KeyRemapScreen>(PushedWidget);
				CreatedKeyRemapScreen->OnKeyRemapScreenKeyPressed.BindUObject(this, &USL_Widget_ListEntry_KeyRemap::OnKeyToRemapPressed);
				CreatedKeyRemapScreen->OnKeyRemapScreenKeySelectCanceled.BindUObject(this, &USL_Widget_ListEntry_KeyRemap::OnKeyRemapCanceled);
				
				if (CachedOwningKeyRemapDataObject)
				{
					CreatedKeyRemapScreen->SetDesiredInputTypeToFilter(CachedOwningKeyRemapDataObject->GetDesiredInputKeyType());
				}
			}
		}
	);
}

void USL_Widget_ListEntry_KeyRemap::OnResetKeyBindingButtonClicked()
{
	SelectThisEntryWidget();
}

void USL_Widget_ListEntry_KeyRemap::OnKeyToRemapPressed(const FKey& PressedKey)
{
	if (CachedOwningKeyRemapDataObject)
	{
		CachedOwningKeyRemapDataObject->BindNewInputKey(PressedKey);
	}
}

void USL_Widget_ListEntry_KeyRemap::OnKeyRemapCanceled(const FString& CanceledReason)
{
	USL_UISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
		ESL_ConfirmScreenType::Ok,
		FText::FromString(TEXT("Key Remap")),
		FText::FromString(CanceledReason),
		[](ESL_ConfirmScreenButtonType ClickedButton) { }
	);
}