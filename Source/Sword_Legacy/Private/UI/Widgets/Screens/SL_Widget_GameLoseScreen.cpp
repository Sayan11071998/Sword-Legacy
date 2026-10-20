#include "UI/Widgets/Screens/SL_Widget_GameLoseScreen.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/Components/SL_CommonButtonBase.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_Widget_GameLoseScreen::USL_Widget_GameLoseScreen()
{
	bIsModal = true;
	bSupportsActivationFocus = true;
}

void USL_Widget_GameLoseScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CommonButton_Retry)
	{
		CommonButton_Retry->OnClicked().AddUObject(this, &USL_Widget_GameLoseScreen::OnRetryClicked);
	}

	if (CommonButton_MainMenu)
	{
		CommonButton_MainMenu->OnClicked().AddUObject(this, &USL_Widget_GameLoseScreen::OnMainMenuClicked);
	}
}

UWidget* USL_Widget_GameLoseScreen::NativeGetDesiredFocusTarget() const
{
	return CommonButton_Retry;
}

TOptional<FUIInputConfig> USL_Widget_GameLoseScreen::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

void USL_Widget_GameLoseScreen::OnRetryClicked()
{
	const FName CurrentLevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
	
	DeactivateWidget();
	
	USL_FunctionLibrary::OpenLevelByNameWithLoadingScreen(this, CurrentLevelName);
}

void USL_Widget_GameLoseScreen::OnMainMenuClicked()
{
	DeactivateWidget();
	
	USL_FunctionLibrary::OpenLevelByNameWithLoadingScreen(this, FName(TEXT("MainMenu")));
}