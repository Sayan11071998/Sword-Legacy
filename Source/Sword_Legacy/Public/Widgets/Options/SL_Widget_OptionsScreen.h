#pragma once

#include "CoreMinimal.h"
#include "Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_OptionsScreen.generated.h"

class USL_TabListWidgetBase;
class USL_OptionsDataRegistry;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_OptionsScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~ End UUserWidget Interface
	
	// ~ Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	// ~ End UCommonActivatableWidget Interface
	
private:
	TObjectPtr<USL_OptionsDataRegistry> GetOrCreateDataRegistry();
	
	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();
	
	// Bound Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_TabListWidgetBase> TabListWidget_OptionsTabs;
	
	// Handle the creation of data registry in options screen. Direct access to this variable is forbidden.
	UPROPERTY(Transient)
	TObjectPtr<USL_OptionsDataRegistry> CreatedOwningDataRegistry;
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	
	UPROPERTY()
	FUIActionBindingHandle ResetActionHandle;
};