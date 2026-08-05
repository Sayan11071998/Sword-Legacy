#include "Widgets/SL_Widget_ConfirmScreen.h"

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