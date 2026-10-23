#include "UI/Widgets/Screens/SL_Widget_GameThankYouScreen.h"
#include "CommonTextBlock.h"
#include "Controllers/SL_PlayerController.h"
#include "TimerManager.h"

USL_Widget_GameThankYouScreen::USL_Widget_GameThankYouScreen()
{
	bIsModal = true;
	bIsBackHandler = false;
	bSupportsActivationFocus = false;
	ThankYouText = FText::FromString(TEXT("Thank you"));
}

void USL_Widget_GameThankYouScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CommonTextBlock_Message)
	{
		CommonTextBlock_Message->SetText(ThankYouText);
	}
}

void USL_Widget_GameThankYouScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (bAdvancingToCredits) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			HoldTimerHandle,
			this,
			&USL_Widget_GameThankYouScreen::ShowCredits,
			ThankYouHoldSeconds,
			false
		);
	}
}

TOptional<FUIInputConfig> USL_Widget_GameThankYouScreen::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

void USL_Widget_GameThankYouScreen::ShowCredits()
{
	if (bAdvancingToCredits) return;

	bAdvancingToCredits = true;

	if (ASL_PlayerController* PlayerController = GetOwningPlayer<ASL_PlayerController>())
	{
		PlayerController->ShowEndingCredits(this);
	}
}