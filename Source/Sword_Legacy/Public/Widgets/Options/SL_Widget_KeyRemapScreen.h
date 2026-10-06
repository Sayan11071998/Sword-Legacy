#pragma once

#include "CoreMinimal.h"
#include "Widgets/SL_Widget_Activatable_Base.h"
#include "CommonInputTypeEnum.h"
#include "SL_Widget_KeyRemapScreen.generated.h"

class UCommonRichTextBlock;
class FSL_KeyRemapScreenInputProcessor;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_KeyRemapScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()
	
public:
	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeyPressedDelegate, const FKey& /*PressedKey*/);
	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeySelectCanceledDelegate, const FString& /*CanceledReason*/);
	
	void SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType);
	
	FOnKeyRemapScreenKeyPressedDelegate OnKeyRemapScreenKeyPressed;
	FOnKeyRemapScreenKeySelectCanceledDelegate OnKeyRemapScreenKeySelectCanceled;
	
protected:
	// ~ Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	// ~ End UCommonActivatableWidget Interface
	
private:
	void OnValidKeyPressedDetected(const FKey& PressedKey);
	void OnKeySelectCanceled(const FString& CanceledReason);
	
	// Delay a tick to make sure the input key is captured properly before calling the PreDeactivateCallback and deactivating the widget
	void RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback);
	
	// Bound Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichText_RemapMessage;

	// Cached Key Remap Input Processor
	TSharedPtr<FSL_KeyRemapScreenInputProcessor> CachedInputPreprocessor;
	
	ECommonInputType CachedDesiredInputType;
};