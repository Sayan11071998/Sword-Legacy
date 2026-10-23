#include "UI/Widgets/Screens/SL_Widget_GamePauseScreen.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Containers/Ticker.h"
#include "Controllers/SL_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Subsystems/SL_UISubsystem.h"
#include "UI/Widgets/Components/SL_CommonButtonBase.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_Widget_GamePauseScreen::USL_Widget_GamePauseScreen()
{
	bIsModal = true;
	bIsBackHandler = true;
	bSupportsActivationFocus = true;
}

void USL_Widget_GamePauseScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CommonButton_Resume)
	{
		CommonButton_Resume->OnClicked().AddUObject(this, &USL_Widget_GamePauseScreen::ClosePause);
	}

	if (CommonButton_Options)
	{
		CommonButton_Options->OnClicked().AddUObject(this, &USL_Widget_GamePauseScreen::OnOptionsClicked);
	}

	if (CommonButton_MainMenu)
	{
		CommonButton_MainMenu->OnClicked().AddUObject(this, &USL_Widget_GamePauseScreen::OnMainMenuClicked);
	}
}

FReply USL_Widget_GamePauseScreen::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!InKeyEvent.IsRepeat() && IsPauseKey(InKeyEvent.GetKey()))
	{
		ClosePause();
		
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void USL_Widget_GamePauseScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	bEndingPause = false;
}

void USL_Widget_GamePauseScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (!bEndingPause) return;

	// World time is dilated to 0 while paused, so a world timer never expires.
	TWeakObjectPtr<ASL_PlayerController> WeakPlayerController(GetOwningPlayerController());
	
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
		[WeakPlayerController](float)
		{
			if (ASL_PlayerController* PlayerController = WeakPlayerController.Get())
			{
				PlayerController->EndGamePause();
			}

			return false;
		}
	));
}

UWidget* USL_Widget_GamePauseScreen::NativeGetDesiredFocusTarget() const
{
	return CommonButton_Resume;
}

TOptional<FUIInputConfig> USL_Widget_GamePauseScreen::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

bool USL_Widget_GamePauseScreen::NativeOnHandleBackAction()
{
	ClosePause();
	
	return true;
}

void USL_Widget_GamePauseScreen::ClosePause()
{
	if (bEndingPause) return;

	bEndingPause = true;
	
	DeactivateWidget();
}

void USL_Widget_GamePauseScreen::OnOptionsClicked()
{
	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	if (!UISubsystem) return;

	const TSoftClassPtr<USL_Widget_Activatable_Base> OptionsScreenClass = USL_FunctionLibrary::GetGameSoftWidgetClassByTag(SL_GameplayTags::UI_Widget_OptionsScreen);
	if (OptionsScreenClass.IsNull()) return;

	TWeakObjectPtr<ASL_PlayerController> WeakPlayerController(GetOwningPlayerController());

	UISubsystem->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_GameMenu,
		OptionsScreenClass,
		[WeakPlayerController](EAsyncPushWidgetState InPushState, TObjectPtr<USL_Widget_Activatable_Base> PushedWidget)
		{
			if (!PushedWidget) return;

			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				if (ASL_PlayerController* PlayerController = WeakPlayerController.Get())
				{
					PushedWidget->SetOwningPlayer(PlayerController);
				}
			}
			else if (InPushState == EAsyncPushWidgetState::AfterPush)
			{
				if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
				{
					WidgetToFocus->SetFocus();
				}
			}
		}
	);
}

void USL_Widget_GamePauseScreen::OnMainMenuClicked()
{
	ClosePause();
	
	USL_FunctionLibrary::OpenLevelByTagWithLoadingScreen(this, SL_GameplayTags::Game_Level_MainMenu);
}

bool USL_Widget_GamePauseScreen::IsPauseKey(const FKey& InKey) const
{
	const ASL_PlayerCharacter* PlayerCharacter = GetOwningPlayerPawn<ASL_PlayerCharacter>();
	if (!PlayerCharacter) return false;

	const UInputAction* PauseAction = PlayerCharacter->GetNativeInputActionByTag(SL_GameplayTags::InputTag_Pause);
	const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	
	if (!PauseAction || !LocalPlayer) return false;

	const UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem) return false;

	return InputSubsystem->QueryKeysMappedToAction(PauseAction).Contains(InKey);
}