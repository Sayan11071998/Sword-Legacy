#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "SL_DeveloperSettings.generated.h"

class USL_Widget_Activatable_Base;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game UI Settings"))
class SWORD_LEGACY_API USL_DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta = (ForceInLineRow, Categories = "UI.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<USL_Widget_Activatable_Base>> GameWidgetMap;
	
	UPROPERTY(Config, EditAnywhere, Category = "Widget Image Reference", meta = (ForceInLineRow, Categories = "UI.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath MasterSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath MusicSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath SoundFXSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = (AllowedClasses = "/Script/Engine.SoundMix"))
	FSoftObjectPath DefaultSoundMix;
};