#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "SL_ListDataObject_KeyRemap.generated.h"

class UEnhancedPlayerMappableKeyProfile;
class UEnhancedInputUserSettings;

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_KeyRemap : public USL_ListDataObject_Base
{
	GENERATED_BODY()
	
public:
	void InitKeyRemapData(
		TObjectPtr<UEnhancedInputUserSettings> InOwningInputUserSettings,
		TObjectPtr<UEnhancedPlayerMappableKeyProfile> InKeyProfile,
		ECommonInputType InDesiredInputKeyType,
		const FPlayerKeyMapping& InOwningPlayerKeyMapping
	);
	
	FSlateBrush GetIconFromCurrentKey() const;
	
private:
	FPlayerKeyMapping* GetOwningKeyMapping() const;
	
	UPROPERTY(Transient)
	TObjectPtr<UEnhancedInputUserSettings> CachedOwningInputUserSettings;
	
	UPROPERTY(Transient)
	TObjectPtr<UEnhancedPlayerMappableKeyProfile> CachedOwningKeyProfile;
	
	UPROPERTY()
	ECommonInputType CachedDesiredInputKeyType;
	
	UPROPERTY()
	FName CachedOwningMappingName;
	
	UPROPERTY()
	EPlayerMappableKeySlot CachedOwningMappableKeySlot;
};