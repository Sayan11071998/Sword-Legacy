#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_GamePauseScreen.generated.h"

class USL_CommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_GamePauseScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()

public:
	USL_Widget_GamePauseScreen();

protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	// ~ End UUserWidget Interface

	// ~ Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual bool NativeOnHandleBackAction() override;
	// ~ End UCommonActivatableWidget Interface

private:
	void ClosePause();
	void OnOptionsClicked();
	void OnMainMenuClicked();
	bool IsPauseKey(const FKey& InKey) const;

	// Bound Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_CommonButtonBase> CommonButton_Resume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_CommonButtonBase> CommonButton_Options;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_CommonButtonBase> CommonButton_MainMenu;

	bool bEndingPause = false;
};
