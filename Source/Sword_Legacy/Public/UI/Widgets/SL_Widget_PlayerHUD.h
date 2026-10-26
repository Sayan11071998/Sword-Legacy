#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_PlayerHUD.generated.h"

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_PlayerHUD : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()

public:
	USL_Widget_PlayerHUD();

protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	// ~ End UUserWidget Interface
};