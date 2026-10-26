#include "UI/Widgets/Screens/SL_Widget_EndingPressAnyKeyScreen.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_Widget_EndingPressAnyKeyScreen::USL_Widget_EndingPressAnyKeyScreen()
{
	bIsModal = true;
	bIsBackHandler = true;
	bIsBackActionDisplayedInActionBar = false;
	bSupportsActivationFocus = true;
	
	SetIsFocusable(true);
}

void USL_Widget_EndingPressAnyKeyScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	SetVisibility(ESlateVisibility::Visible);
}

void USL_Widget_EndingPressAnyKeyScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	SetFocus();
}

UWidget* USL_Widget_EndingPressAnyKeyScreen::NativeGetDesiredFocusTarget() const
{
	return const_cast<USL_Widget_EndingPressAnyKeyScreen*>(this);
}

TOptional<FUIInputConfig> USL_Widget_EndingPressAnyKeyScreen::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

FReply USL_Widget_EndingPressAnyKeyScreen::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!InKeyEvent.IsRepeat())
	{
		OpenMainMenu();
	}

	return FReply::Handled();
}

FReply USL_Widget_EndingPressAnyKeyScreen::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OpenMainMenu();
	
	return FReply::Handled();
}

bool USL_Widget_EndingPressAnyKeyScreen::NativeOnHandleBackAction()
{
	OpenMainMenu();
	
	return true;
}

void USL_Widget_EndingPressAnyKeyScreen::OpenMainMenu()
{
	if (bOpeningMainMenu) return;

	bOpeningMainMenu = true;
	
	DeactivateWidget();
	
	USL_FunctionLibrary::OpenLevelByTagWithLoadingScreen(this, SL_GameplayTags::Game_Level_MainMenu);
}