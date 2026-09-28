#include "Utilities/SL_GameUserSettings.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "UObject/UObjectIterator.h"
#include "Utilities/SL_SoundDeveloperSettings.h"

USL_GameUserSettings::USL_GameUserSettings() :
	OverallVolume(1.f),
	MusicVolume(1.f),
	SoundFXVolume(1.f),
	bAllowBackgroundAudio(false),
	bUseHDRAudioMode(false)
{ }

TObjectPtr<USL_GameUserSettings> USL_GameUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<USL_GameUserSettings>(GEngine->GetGameUserSettings());
	}
	
	return nullptr;
}

void USL_GameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
	ApplyCurrentGameDifficultyToAbilitySystems();
	SetOverallVolume(OverallVolume);
}

int32 USL_GameUserSettings::GetCurrentGameDifficultyAsAbilityLevel() const
{
	if (CurrentGameDifficulty.Equals(TEXT("Easy"), ESearchCase::IgnoreCase))
	{
		return 1;
	}

	if (CurrentGameDifficulty.Equals(TEXT("Hard"), ESearchCase::IgnoreCase))
	{
		return 3;
	}

	if (CurrentGameDifficulty.Equals(TEXT("Very Hard"), ESearchCase::IgnoreCase))
	{
		return 4;
	}

	return 2;
}

void USL_GameUserSettings::ApplyCurrentGameDifficultyToAbilitySystems() const
{
	if (!GEngine) return;

	const int32 AbilityLevel = GetCurrentGameDifficultyAsAbilityLevel();

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		
		if (!World || !World->IsGameWorld()) continue;

		for (TObjectIterator<USL_AbilitySystemComponent> It; It; ++It)
		{
			USL_AbilitySystemComponent* AbilitySystemComponent = *It;
			if (!AbilitySystemComponent || AbilitySystemComponent->GetWorld() != World) continue;

			AbilitySystemComponent->SetGrantedAbilityLevels(AbilityLevel);
		}
	}
}

void USL_GameUserSettings::SetCurrentGameDifficulty(const FString& InNewDifficulty)
{
	CurrentGameDifficulty = InNewDifficulty;
	ApplyCurrentGameDifficultyToAbilitySystems();
}

void USL_GameUserSettings::SetOverallVolume(float InVolume)
{
	OverallVolume = InVolume;

	UWorld* InAudioWorld = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
	const USL_SoundDeveloperSettings* SoundSettings = GetDefault<USL_SoundDeveloperSettings>();

	if (!InAudioWorld || !SoundSettings) return;

	USoundClass* LoadedMasterSoundClass = Cast<USoundClass>(SoundSettings->MasterSoundClass.TryLoad());
	USoundMix* LoadedDefaultSoundMix = Cast<USoundMix>(SoundSettings->DefaultSoundMix.TryLoad());

	if (!LoadedMasterSoundClass || !LoadedDefaultSoundMix) return;

	UGameplayStatics::SetSoundMixClassOverride(InAudioWorld, LoadedDefaultSoundMix, LoadedMasterSoundClass, OverallVolume, 1.f, 0.2f);
	UGameplayStatics::PushSoundMixModifier(InAudioWorld, LoadedDefaultSoundMix);
}

void USL_GameUserSettings::SetMusicVolume(float InVolume)
{
	MusicVolume = InVolume;
}

void USL_GameUserSettings::SetSoundFXVolume(float InVolume)
{
	SoundFXVolume = InVolume;
}

void USL_GameUserSettings::SetAllowBackgroundAudio(bool bIsAllowed)
{
	bAllowBackgroundAudio = bIsAllowed;
}

void USL_GameUserSettings::SetUseHDRAudioMode(bool bIsAllowed)
{
	bUseHDRAudioMode = bIsAllowed;
}