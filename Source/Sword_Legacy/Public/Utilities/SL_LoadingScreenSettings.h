#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SL_LoadingScreenSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class SWORD_LEGACY_API USL_LoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	TSubclassOf<UUserWidget> GetLoadingScreenWidgetClassChecked() const;
	
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	TSoftClassPtr<UUserWidget> SoftLoadingScreenWidgetClass;
	
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	float HoldLoadingScreenExtraSeconds = 3.f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	bool bShowLoadingScreenInEditor = false;
};