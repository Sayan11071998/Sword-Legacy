#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_GameThankYouScreen.generated.h"

class UCommonTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_GameThankYouScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()

public:
	USL_Widget_GameThankYouScreen();

protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~ End UUserWidget Interface

	// ~ Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	// ~ End UCommonActivatableWidget Interface

	UPROPERTY(EditDefaultsOnly, Category = "Thank You")
	FText ThankYouText;

	UPROPERTY(EditDefaultsOnly, Category = "Thank You", meta = (ClampMin = "0.5", Units = "s"))
	float ThankYouHoldSeconds = 5.f;

private:
	void ShowCredits();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_Message;

	FTimerHandle HoldTimerHandle;
	bool bAdvancingToCredits = false;
};