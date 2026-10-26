#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_EndingPressAnyKeyScreen.generated.h"

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_EndingPressAnyKeyScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()

public:
	USL_Widget_EndingPressAnyKeyScreen();

protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	// ~ End UUserWidget Interface

	// ~ Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual bool NativeOnHandleBackAction() override;
	// ~ End UCommonActivatableWidget Interface

private:
	void OpenMainMenu();

	bool bOpeningMainMenu = false;
};