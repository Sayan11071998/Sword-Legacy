#include "Widgets/SL_Widget_ConfirmScreen.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Widgets/Components/SL_CommonButtonBase.h"
#include "ICommonInputModule.h"

TObjectPtr<USL_ConfirmScreenInfoObject> USL_ConfirmScreenInfoObject::CreateOKScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	USL_ConfirmScreenInfoObject* InfoObject = NewObject<USL_ConfirmScreenInfoObject>();
	
	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;
	
	FSL_ConfirmButtonScreenInfo OKButtonInfo;
	
	OKButtonInfo.ConfirmScreenButtonType = ESL_ConfirmScreenButtonType::Closed;
	OKButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Ok"));
	
	InfoObject->AvailableScreenButtons.Add(OKButtonInfo);
	
	return InfoObject;
}

TObjectPtr<USL_ConfirmScreenInfoObject> USL_ConfirmScreenInfoObject::CreateYesNoScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	USL_ConfirmScreenInfoObject* InfoObject = NewObject<USL_ConfirmScreenInfoObject>();
	
	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;
	
	FSL_ConfirmButtonScreenInfo YesButtonInfo;
	
	YesButtonInfo.ConfirmScreenButtonType = ESL_ConfirmScreenButtonType::Confirmed;
	YesButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Yes"));
	
	FSL_ConfirmButtonScreenInfo NoButtonInfo;
	
	NoButtonInfo.ConfirmScreenButtonType = ESL_ConfirmScreenButtonType::Cancelled;
	NoButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("No"));
	
	InfoObject->AvailableScreenButtons.Add(YesButtonInfo);
	InfoObject->AvailableScreenButtons.Add(NoButtonInfo);
	
	return InfoObject;
}

TObjectPtr<USL_ConfirmScreenInfoObject> USL_ConfirmScreenInfoObject::CreateOkCancelScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	USL_ConfirmScreenInfoObject* InfoObject = NewObject<USL_ConfirmScreenInfoObject>();
	
	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;
	
	FSL_ConfirmButtonScreenInfo OkButtonInfo;
	
	OkButtonInfo.ConfirmScreenButtonType = ESL_ConfirmScreenButtonType::Closed;
	OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Ok"));
	
	FSL_ConfirmButtonScreenInfo CancelButtonInfo;
	
	CancelButtonInfo.ConfirmScreenButtonType = ESL_ConfirmScreenButtonType::Cancelled;
	CancelButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Cancel"));
	
	InfoObject->AvailableScreenButtons.Add(OkButtonInfo);
	InfoObject->AvailableScreenButtons.Add(CancelButtonInfo);
	
	return InfoObject;
}

void USL_Widget_ConfirmScreen::InitConfirmScreen(TObjectPtr<USL_ConfirmScreenInfoObject> InScreenInfoObject,
	TFunction<void(ESL_ConfirmScreenButtonType)> ClickedButtonCallback)
{
	check(InScreenInfoObject && CommonTextBlock_Title && CommonTextBlock_Message && DynamicEntryBox_Buttons);
	
	CommonTextBlock_Title->SetText(InScreenInfoObject->ScreenTitle);
	CommonTextBlock_Message->SetText(InScreenInfoObject->ScreenMessage);
	
	// Checking if the entry box has old buttons created previously
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		// Clearing the old buttons the entry box has. The widget type for the entry box is specified in the child widget blueprint
		DynamicEntryBox_Buttons->Reset<USL_CommonButtonBase>(
			[](USL_CommonButtonBase& ExistingButton)
			{
				ExistingButton.OnClicked().Clear();
			}
		);
	}
	
	check(!InScreenInfoObject->AvailableScreenButtons.IsEmpty());
	
	for (FSL_ConfirmButtonScreenInfo& AvailableButtonInfo : InScreenInfoObject->AvailableScreenButtons)
	{
		FDataTableRowHandle InputActionRowHandle;
		
		switch (AvailableButtonInfo.ConfirmScreenButtonType)
		{
		case ESL_ConfirmScreenButtonType::Confirmed:
			InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultClickAction();
			break;
			
		case ESL_ConfirmScreenButtonType::Cancelled:
			InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();
			break;
			
		case ESL_ConfirmScreenButtonType::Closed:
			InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();
			break;
			
		default:
			break;
		}
		
		USL_CommonButtonBase* AddedButton = DynamicEntryBox_Buttons->CreateEntry<USL_CommonButtonBase>();
		
		AddedButton->SetButtonText(AvailableButtonInfo.ButtonTextToDisplay);
		AddedButton->SetTriggeredInputAction(InputActionRowHandle);
		
		AddedButton->OnClicked().AddLambda(
			[ClickedButtonCallback, AvailableButtonInfo, this]()
			{
				ClickedButtonCallback(AvailableButtonInfo.ConfirmScreenButtonType);
				
				DeactivateWidget();
			}
		);
	}
	
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		// Set focus on the last button. So, if there are two buttons, one is Yes, one is No, our gamepad will focus on No button.
		DynamicEntryBox_Buttons->GetAllEntries().Last()->SetFocus();
	}
}