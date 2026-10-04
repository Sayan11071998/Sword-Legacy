#include "Widgets/Options/DataObjects/SL_ListDataObject_KeyRemap.h"

void USL_ListDataObject_KeyRemap::InitKeyRemapData(TObjectPtr<UEnhancedInputUserSettings> InOwningInputUserSettings,
	TObjectPtr<UEnhancedPlayerMappableKeyProfile> InKeyProfile, ECommonInputType InDesiredInputKeyType,
	const FPlayerKeyMapping& InOwningPlayerKeyMapping)
{
	CachedOwningInputUserSettings = InOwningInputUserSettings;
	CachedOwningKeyProfile = InKeyProfile;
	CachedDesiredInputKeyType = InDesiredInputKeyType;
	CachedOwningMappingName = InOwningPlayerKeyMapping.GetMappingName();
	CachedOwningMappableKeySlot = InOwningPlayerKeyMapping.GetSlot();
}