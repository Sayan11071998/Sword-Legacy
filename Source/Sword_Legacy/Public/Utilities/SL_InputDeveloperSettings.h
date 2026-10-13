#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SL_InputDeveloperSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game Input Settings"))
class SWORD_LEGACY_API USL_InputDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Mapping Contexts", meta = (AllowedClasses = "/Script/EnhancedInput.InputMappingContext"))
	TArray<FSoftObjectPath> PlayerMappableMappingContexts;
};