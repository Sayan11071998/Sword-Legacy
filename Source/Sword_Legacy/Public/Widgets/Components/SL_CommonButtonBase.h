#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "SL_CommonButtonBase.generated.h"

class UCommonTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_CommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText InText);
	
private:
	// ~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	// ~ End UUserWidget Interface
	
	// ~ Begin UCommonButtonBase Interface
	virtual void NativeOnCurrentTextStyleChanged() override;
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;
	// ~ End UCommonButtonBase Interface
	
	// **** Bound Widgets ****//
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_ButtonText;
	// **** Bound Widgets ****//
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDisplayText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Button", meta = (AllowPrivateAccess = "true"))
	bool bUseUpperCaseForButtonText = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDescriptionText;
};