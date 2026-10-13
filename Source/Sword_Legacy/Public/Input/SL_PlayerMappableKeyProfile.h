#pragma once

#include "UserSettings/EnhancedInputUserSettings.h"
#include "SL_PlayerMappableKeyProfile.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_PlayerMappableKeyProfile : public UEnhancedPlayerMappableKeyProfile
{
	GENERATED_BODY()

public:
	// ~ Begin UEnhancedPlayerMappableKeyProfile Interface
	virtual int32 GetPlayerMappedKeysForRebuildControlMappings(const FEnhancedActionKeyMapping& DefaultMapping, TArray<FKey>& OutKeys) const override;
	// ~ End UEnhancedPlayerMappableKeyProfile Interface
};