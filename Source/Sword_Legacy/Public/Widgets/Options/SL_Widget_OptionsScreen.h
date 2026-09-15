#pragma once

#include "CoreMinimal.h"
#include "Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_OptionsScreen.generated.h"

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_OptionsScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~ End UUserWidget Interface
	
private:
	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	
	UPROPERTY()
	FUIActionBindingHandle ResetActionHandle;
};