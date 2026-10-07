#include "Widgets/Options/SL_Widget_KeyRemapScreen.h"
#include "CommonRichTextBlock.h"
#include "Framework/Application/IInputProcessor.h"
#include "CommonInputSubsystem.h"
#include "CommonUITypes.h"
#include "ICommonInputModule.h"

// *** FSL_KeyRemapScreenInputProcessor ***//
class FSL_KeyRemapScreenInputProcessor : public IInputProcessor
{
public:
	FSL_KeyRemapScreenInputProcessor(ECommonInputType InInputTypeToListenTo, ULocalPlayer* InOwningLocalPlayer)
		: CachedInputTypeToListenTo(InInputTypeToListenTo),
		  CachedWeakOwningLocalPlayer(InOwningLocalPlayer) { }
	
	DECLARE_DELEGATE_OneParam(FOnInputPreprocessorKeyPressedDelegate, const FKey& /*PressedKey*/);
	DECLARE_DELEGATE_OneParam(FOnInputPreprocessorKeySelectCanceledDelegate, const FString& /*CanceledReason*/);
	
	FOnInputPreprocessorKeyPressedDelegate OnInputPreprocessorKeyPressed;
	FOnInputPreprocessorKeySelectCanceledDelegate OnInputPreprocessorKeySelectCanceled;
	
protected:
	// ~ Begin IInputProcessor Interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }
	
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		ProcessPressedKey(InKeyEvent.GetKey());
		
		return true;
	}
	
	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		ProcessPressedKey(MouseEvent.GetEffectingButton());
		
		return true;
	}
	// ~ End IInputProcessor Interface
	
	void ProcessPressedKey(const FKey& InPressedKey)
	{
		if (InPressedKey == EKeys::Escape)
		{
			OnInputPreprocessorKeySelectCanceled.ExecuteIfBound(TEXT("Key Remap Has Been Canceled."));
		}
		
		UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedWeakOwningLocalPlayer.Get());
		
		check(CommonInputSubsystem);
		
		ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();

		switch (CachedInputTypeToListenTo)
		{
		case ECommonInputType::MouseAndKeyboard:
			if (InPressedKey.IsGamepadKey() || CurrentInputType == ECommonInputType::Gamepad)
			{
				OnInputPreprocessorKeySelectCanceled.ExecuteIfBound(TEXT("Detected Gamepad Key Pressed For Keyboard Inputs. Key Remap Has Been Canceled"));
				return;
			}
			break;
			
		case ECommonInputType::Gamepad:
			if (CurrentInputType == ECommonInputType::Gamepad && InPressedKey == EKeys::LeftMouseButton)
			{
				FCommonInputActionDataBase* InputActionData = ICommonInputModule::GetSettings().GetDefaultClickAction().GetRow<FCommonInputActionDataBase>(TEXT(""));
				
				check(InputActionData);
				
				OnInputPreprocessorKeyPressed.ExecuteIfBound(InputActionData->GetDefaultGamepadInputTypeInfo().GetKey());
				
				return;
			}
			
			if (!InPressedKey.IsGamepadKey())
			{
				OnInputPreprocessorKeySelectCanceled.ExecuteIfBound(TEXT("Detected Non-Gamepad Key Pressed For Gamepad Inputs. Key Remap Has Been Canceled"));
				return;
			}
			break;
		
		default:
			break;
		}
		
		OnInputPreprocessorKeyPressed.ExecuteIfBound(InPressedKey);
	}
	
private:
	ECommonInputType CachedInputTypeToListenTo;
	TWeakObjectPtr<ULocalPlayer> CachedWeakOwningLocalPlayer;
};
// *** FSL_KeyRemapScreenInputProcessor ***//

// *** USL_Widget_KeyRemapScreen ***//
void USL_Widget_KeyRemapScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	CachedInputPreprocessor = MakeShared<FSL_KeyRemapScreenInputProcessor>(CachedDesiredInputType, GetOwningLocalPlayer());
	
	CachedInputPreprocessor->OnInputPreprocessorKeyPressed.BindUObject(this, &USL_Widget_KeyRemapScreen::OnValidKeyPressedDetected);
	CachedInputPreprocessor->OnInputPreprocessorKeySelectCanceled.BindUObject(this, &USL_Widget_KeyRemapScreen::OnKeySelectCanceled);
	
	FSlateApplication::Get().RegisterInputPreProcessor(CachedInputPreprocessor, -1);
	
	FString InputDeviceName;

	switch (CachedDesiredInputType)
	{
	case ECommonInputType::MouseAndKeyboard:
		InputDeviceName = TEXT("Mouse & Keyboard");
		break;
		
	case ECommonInputType::Gamepad:
		InputDeviceName = TEXT("Gamepad");
		break;
		
	default:
		break;
	}
	
	const FString DisplayRichMessage = FString::Printf(
		TEXT("<KeyRemapDefault>Press any</> <KeyRemapHighlight>%s</> <KeyRemapDefault>Key</>"), *InputDeviceName	
	);
	
	CommonRichText_RemapMessage->SetText(FText::FromString(DisplayRichMessage));
}

void USL_Widget_KeyRemapScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	if (CachedInputPreprocessor)
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(CachedInputPreprocessor);
		
		CachedInputPreprocessor.Reset();
	}
}

void USL_Widget_KeyRemapScreen::OnValidKeyPressedDetected(const FKey& PressedKey)
{
	RequestDeactivateWidget(
		[this, PressedKey]()
		{
			OnKeyRemapScreenKeyPressed.ExecuteIfBound(PressedKey);
		}
	);
}

void USL_Widget_KeyRemapScreen::OnKeySelectCanceled(const FString& CanceledReason)
{
	RequestDeactivateWidget(
		[this, CanceledReason]()
		{
			OnKeyRemapScreenKeySelectCanceled.ExecuteIfBound(CanceledReason);
		}
	);
}

void USL_Widget_KeyRemapScreen::RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback)
{
	// Delay a tick to make sure the input is processed correctly
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda(
			[PreDeactivateCallback, this](float DeltaTime)->bool
			{
				PreDeactivateCallback();
				
				DeactivateWidget();
				
				return false;
			}
		)
	);
}

void USL_Widget_KeyRemapScreen::SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType)
{
	CachedDesiredInputType = InDesiredInputType;
}
// *** USL_Widget_KeyRemapScreen ***//