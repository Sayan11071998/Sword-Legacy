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

	// How long a cleared level stays on screen before the next level loads. The player cannot act during this hold
	UPROPERTY(Config, EditAnywhere, Category = "Level Progression", meta = (ClampMin = "0.0", Units = "s"))
	float LevelClearHoldSeconds = 2.5f;

	// Camera fade to black when a level ends, and back in when the next level starts
	UPROPERTY(Config, EditAnywhere, Category = "Level Progression", meta = (ClampMin = "0.0", Units = "s"))
	float LevelTransitionFadeSeconds = 1.f;

	// How long the Game HUD takes to fade out when a level is cleared, and fade in when the next level starts
	UPROPERTY(Config, EditAnywhere, Category = "Level Progression", meta = (ClampMin = "0.0", Units = "s"))
	float GameHUDFadeSeconds = 1.5f;
};