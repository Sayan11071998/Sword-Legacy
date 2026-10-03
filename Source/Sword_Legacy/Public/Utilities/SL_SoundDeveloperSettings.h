#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SL_SoundDeveloperSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game Sound Settings"))
class SWORD_LEGACY_API USL_SoundDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath MasterSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath MusicSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath SoundFXSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Sound Mix", meta = (AllowedClasses = "/Script/Engine.SoundMix"))
	FSoftObjectPath DefaultSoundMix;
};