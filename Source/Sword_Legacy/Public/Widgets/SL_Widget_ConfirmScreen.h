#pragma once

#include "CoreMinimal.h"
#include "Widgets/SL_Widget_Activatable_Base.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_Widget_ConfirmScreen.generated.h"

class UDynamicEntryBox;
class UCommonTextBlock;

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