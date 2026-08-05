#pragma once

#include "CoreMinimal.h"
#include "Widgets/SL_Widget_Activatable_Base.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_Widget_ConfirmScreen.generated.h"

class UDynamicEntryBox;
class UCommonTextBlock;

USTRUCT(BlueprintType)
struct FSL_ConfirmButtonScreenInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESL_ConfirmScreenButtonType ConfirmScreenButtonType = ESL_ConfirmScreenButtonType::Unknown;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonTextToDisplay;
};

UCLASS()
class USL_ConfirmScreenInfoObject : public UObject
{
	GENERATED_BODY()
	
public:
	static TObjectPtr<USL_ConfirmScreenInfoObject> CreateOKScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	static TObjectPtr<USL_ConfirmScreenInfoObject> CreateYesNoScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	static TObjectPtr<USL_ConfirmScreenInfoObject> CreateOkCancelScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	
	UPROPERTY(Transient)
	FText ScreenTitle;
	
	UPROPERTY(Transient)
	FText ScreenMessage;
	
	UPROPERTY(Transient)
	TArray<FSL_ConfirmButtonScreenInfo> AvailableScreenButtons;
};

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_ConfirmScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_Title;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_Message;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDynamicEntryBox> DynamicEntryBox_Buttons;
};