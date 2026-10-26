#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_EndingCreditScreen.generated.h"

class UCommonRichTextBlock;
class UScrollBox;

UCLASS(Abstract, BlueprintType)
class SWORD_LEGACY_API USL_Widget_EndingCreditScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()

public:
	USL_Widget_EndingCreditScreen();

protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// ~ End UUserWidget Interface

	// ~ Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	// ~ End UCommonActivatableWidget Interface

	UPROPERTY(EditDefaultsOnly, Category = "Credits", meta = (ClampMin = "1.0"))
	float ScrollSpeed = 40.f;

	UPROPERTY(EditDefaultsOnly, Category = "Credits", meta = (ClampMin = "0.0", Units = "s"))
	float ScrollStartDelay = 1.f;

private:
	void MakeScrollBoxFillParent() const;
	void PadScrollContent(float ViewHeight) const;

	// Bound Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Main;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichTextBlock_Content;

	float ElapsedDelay = 0.f;
	bool bScrollBoxFilled = false;
	bool bLayoutPrepared = false;
	bool bScrollFinished = false;
};