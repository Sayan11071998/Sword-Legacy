#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SL_Widget_OptionsDetailsView.generated.h"

class UCommonRichTextBlock;
class UCommonLazyImage;
class UCommonTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_OptionsDetailsView : public UUserWidget
{
	GENERATED_BODY()
	
private:
	// Bound Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_Title;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> CommonLazyImage_DescriptionImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichText_Description;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichText_DynamicDetailsCommonRichText_DynamicDetails;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichText_DisabledReason;
};