#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "SL_LevelDeveloperSettings.generated.h"

class UWorld;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game Level Settings"))
class SWORD_LEGACY_API USL_LevelDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Level Reference", meta = (ForceInLineRow, Categories = "Game.Level"))
	TMap<FGameplayTag, TSoftObjectPtr<UWorld>> GameLevelMap;
};