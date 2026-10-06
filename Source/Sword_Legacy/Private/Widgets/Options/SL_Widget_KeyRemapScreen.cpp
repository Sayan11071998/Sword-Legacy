#include "Widgets/Options/SL_Widget_KeyRemapScreen.h"
#include "Framework/Application/IInputProcessor.h"
#include "CommonRichTextBlock.h"

#include "SL_DebugHelper.h"

class FSL_KeyRemapScreenInputProcessor : public IInputProcessor
{
protected:
	// ~ Begin IInputProcessor Interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }
	
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		Debug::Print(TEXT("Pressed Key ") + InKeyEvent.GetKey().GetDisplayName().ToString());
		
		return true;
	}
	
	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		Debug::Print(TEXT("Pressed Key ") + MouseEvent.GetEffectingButton().GetDisplayName().ToString());
		
		return true;
	}
	// ~ End IInputProcessor Interface
};


void USL_Widget_KeyRemapScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	CachedInputPreprocessor = MakeShared<FSL_KeyRemapScreenInputProcessor>();
	
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