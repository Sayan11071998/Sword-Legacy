#include "Widgets/Components/SL_CommonButtonBase.h"
#include "CommonTextBlock.h"

void USL_CommonButtonBase::SetButtonText(FText InText)
{
	if (CommonTextBlock_ButtonText && !InText.IsEmpty())
	{
		CommonTextBlock_ButtonText->SetText(bUseUpperCaseForButtonText ? InText.ToUpper() : InText);
	}
}

void USL_CommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	SetButtonText(ButtonDisplayText);
}

void USL_CommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();
	
	if (CommonTextBlock_ButtonText)
	{
		CommonTextBlock_ButtonText->SetStyle(GetCurrentTextStyleClass());
	}
}