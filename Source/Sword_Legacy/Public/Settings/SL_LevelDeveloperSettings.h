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

	// Combat levels in play order. Clearing one loads the next. The final level does not load another level
	UPROPERTY(Config, EditAnywhere, Category = "Level Progression", meta = (Categories = "Game.Level"))
	TArray<FGameplayTag> LevelProgression;
};