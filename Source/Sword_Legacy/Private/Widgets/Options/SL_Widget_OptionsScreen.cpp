#include "Widgets/Options/SL_Widget_OptionsScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"

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
}

void USL_Widget_OptionsScreen::OnResetBoundActionTriggered()
{
	Debug::Print(TEXT("Reset Bound Action Triggered"));
}

void USL_Widget_OptionsScreen::OnBackBoundActionTriggered()
{
	DeactivateWidget();
}