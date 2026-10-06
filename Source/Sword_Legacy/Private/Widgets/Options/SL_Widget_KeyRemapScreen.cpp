#include "Widgets/Options/SL_Widget_KeyRemapScreen.h"
#include "Framework/Application/IInputProcessor.h"

// *** FSL_KeyRemapScreenInputProcessor ***//
class FSL_KeyRemapScreenInputProcessor : public IInputProcessor
{
	
public:
	FSL_KeyRemapScreenInputProcessor(ECommonInputType InInputTypeToListenTo)
		: CachedInputTypeToListenTo(InInputTypeToListenTo) { }
	
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

		switch (CachedInputTypeToListenTo)
		{
		case ECommonInputType::MouseAndKeyboard:
			if (InPressedKey.IsGamepadKey())
			{
				OnInputPreprocessorKeySelectCanceled.ExecuteIfBound(TEXT("Detected Gamepad Key Pressed For Keyboard Inputs. Key Remap Has Been Canceled"));
				return;
			}
			break;
			
		case ECommonInputType::Gamepad:
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
};
// *** FSL_KeyRemapScreenInputProcessor ***//

// *** USL_Widget_KeyRemapScreen ***//
void USL_Widget_KeyRemapScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	CachedInputPreprocessor = MakeShared<FSL_KeyRemapScreenInputProcessor>(CachedDesiredInputType);
	
	FSlateApplication::Get().RegisterInputPreProcessor(CachedInputPreprocessor, -1);
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

void USL_Widget_KeyRemapScreen::SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType)
{
	CachedDesiredInputType = InDesiredInputType;
}
// *** USL_Widget_KeyRemapScreen ***//