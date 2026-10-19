#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_GameLoseScreen.generated.h"

class USL_CommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_GameLoseScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()

protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~ End UUserWidget Interface
	
	// ~ Begin UCommonActivatableWidget Interface
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	// ~ End UCommonActivatableWidget Interface

private:
	void OnRetryClicked();
	void OnMainMenuClicked();

	// Bound Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_CommonButtonBase> CommonButton_Retry;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_CommonButtonBase> CommonButton_MainMenu;
};