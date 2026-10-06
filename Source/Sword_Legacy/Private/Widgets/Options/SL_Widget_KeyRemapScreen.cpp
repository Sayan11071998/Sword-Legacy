#include "Widgets/Options/SL_Widget_KeyRemapScreen.h"
#include "Framework/Application/IInputProcessor.h"
#include "CommonRichTextBlock.h"

#include "SL_DebugHelper.h"

// *** FSL_KeyRemapScreenInputProcessor ***//
class FSL_KeyRemapScreenInputProcessor : public IInputProcessor
{
	
public:
	FSL_KeyRemapScreenInputProcessor(ECommonInputType InInputTypeToListenTo)
		: CachedInputTypeToListenTo(InInputTypeToListenTo) { }
	
protected:
	// ~ Begin IInputProcessor Interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }
	
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		Debug::Print(TEXT("Pressed Key ") + InKeyEvent.GetKey().GetDisplayName().ToString());
		
		UEnum* StaticCommonInputType = StaticEnum<ECommonInputType>();
		
		Debug::Print(TEXT("Desired Input Key Type: ") + StaticCommonInputType->GetValueAsString(CachedInputTypeToListenTo));
		
		return true;
	}
	
	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		Debug::Print(TEXT("Pressed Key ") + MouseEvent.GetEffectingButton().GetDisplayName().ToString());
		
		UEnum* StaticCommonInputType = StaticEnum<ECommonInputType>();
		
		Debug::Print(TEXT("Desired Input Key Type: ") + StaticCommonInputType->GetValueAsString(CachedInputTypeToListenTo));
		
		return true;
	}
	// ~ End IInputProcessor Interface
	
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