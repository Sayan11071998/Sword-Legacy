#include "UI/Widgets/SL_Widget_PlayerHUD.h"

USL_Widget_PlayerHUD::USL_Widget_PlayerHUD()
{
	bIsModal = false;
	bIsBackHandler = false;
	bIsBackActionDisplayedInActionBar = false;
	bSupportsActivationFocus = false;
	bSetVisibilityOnActivated = false;
	bSetVisibilityOnDeactivated = false;
}

void USL_Widget_PlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

TOptional<FUIInputConfig> USL_Widget_PlayerHUD::GetDesiredInputConfig() const
{
	return TOptional<FUIInputConfig>();
}