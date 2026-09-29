#include "Utilities/SL_GameUserSettings.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "SubmixEffects/AudioMixerSubmixEffectDynamicsProcessor.h"
#include "UObject/UObjectIterator.h"
#include "Utilities/SL_SoundDeveloperSettings.h"

void USL_GameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);

	if (!FApp::CanEverRender() || IsRunningDedicatedServer() || IsRunningCommandlet()) return;
	if (GetLastCPUBenchmarkResult() != -1.f && GetLastGPUBenchmarkResult() != -1.f) return;

	RunHardwareBenchmark();
	ApplyHardwareBenchmarkResults();
}

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
	ApplyAllowBackgroundAudio();
	ApplyHDRAudioMode();
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

void USL_GameUserSettings::ApplySoundMixVolumes() const
{
	UWorld* AudioWorld = FindGameAudioWorld();
	const USL_SoundDeveloperSettings* SoundSettings = GetDefault<USL_SoundDeveloperSettings>();
	
	if (!AudioWorld || !SoundSettings) return;

	USoundClass* MasterSoundClass = Cast<USoundClass>(SoundSettings->MasterSoundClass.TryLoad());
	USoundClass* MusicSoundClass = Cast<USoundClass>(SoundSettings->MusicSoundClass.TryLoad());
	USoundClass* SoundFXSoundClass = Cast<USoundClass>(SoundSettings->SoundFXSoundClass.TryLoad());
	USoundMix* DefaultSoundMix = Cast<USoundMix>(SoundSettings->DefaultSoundMix.TryLoad());
	
	if (!DefaultSoundMix) return;

	const bool bMasterDrivesMusic = MasterSoundClass && MusicSoundClass && MasterSoundClass->ChildClasses.Contains(MusicSoundClass);
	const bool bMasterDrivesSoundFX = MasterSoundClass && SoundFXSoundClass && MasterSoundClass->ChildClasses.Contains(SoundFXSoundClass);

	if (bMasterDrivesMusic && bMasterDrivesSoundFX)
	{
		SetSoundClassVolume(AudioWorld, DefaultSoundMix, MasterSoundClass, OverallVolume, true);
		SetSoundClassVolume(AudioWorld, DefaultSoundMix, MusicSoundClass, MusicVolume, true);
		SetSoundClassVolume(AudioWorld, DefaultSoundMix, SoundFXSoundClass, SoundFXVolume, true);
	}
	else
	{
		SetSoundClassVolume(AudioWorld, DefaultSoundMix, MasterSoundClass, OverallVolume, false);
		SetSoundClassVolume(AudioWorld, DefaultSoundMix, MusicSoundClass, OverallVolume * MusicVolume, true);
		SetSoundClassVolume(AudioWorld, DefaultSoundMix, SoundFXSoundClass, OverallVolume * SoundFXVolume, true);
	}

	UGameplayStatics::PushSoundMixModifier(AudioWorld, DefaultSoundMix);
}

void USL_GameUserSettings::ApplyAllowBackgroundAudio() const
{
	FApp::SetUnfocusedVolumeMultiplier(bAllowBackgroundAudio ? 1.f : 0.f);
}

void USL_GameUserSettings::ApplyHDRAudioMode()
{
	UWorld* AudioWorld = FindGameAudioWorld();
	if (!AudioWorld) return;

	if (!bUseHDRAudioMode)
	{
		if (bHDRAudioLimiterActive && HDRAudioLimiter)
		{
			UAudioMixerBlueprintLibrary::RemoveMasterSubmixEffect(AudioWorld, HDRAudioLimiter);
			bHDRAudioLimiterActive = false;
		}
		return;
	}

	if (!HDRAudioLimiter)
	{
		HDRAudioLimiter = NewObject<USubmixEffectDynamicsProcessorPreset>(this);

		FSubmixEffectDynamicsProcessorSettings Settings;
		Settings.DynamicsProcessorType = ESubmixEffectDynamicsProcessorType::Compressor;
		Settings.ThresholdDb = -18.f;
		Settings.Ratio = 8.f;
		Settings.AttackTimeMsec = 5.f;
		Settings.ReleaseTimeMsec = 150.f;
		Settings.KneeBandwidthDb = 6.f;
		Settings.OutputGainDb = 3.f;
		Settings.bAnalogMode = true;
		HDRAudioLimiter->SetSettings(Settings);
	}

	if (!bHDRAudioLimiterActive)
	{
		UAudioMixerBlueprintLibrary::AddMasterSubmixEffect(AudioWorld, HDRAudioLimiter);
		bHDRAudioLimiterActive = true;
	}
}

void USL_GameUserSettings::SetCurrentGameDifficulty(const FString& InNewDifficulty)
{
	CurrentGameDifficulty = InNewDifficulty;
	ApplyCurrentGameDifficultyToAbilitySystems();
}

UWorld* USL_GameUserSettings::FindGameAudioWorld() const
{
	if (!GEngine) return nullptr;

	if (UWorld* PlayWorld = GEngine->GetCurrentPlayWorld()) return PlayWorld;

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		if (World && World->IsGameWorld() && World->bAllowAudioPlayback) return World;
	}

	return nullptr;
}

void USL_GameUserSettings::SetSoundClassVolume(UWorld* AudioWorld, USoundMix* SoundMix, USoundClass* SoundClass,
	float Volume, bool bApplyToChildren) const
{
	if (!AudioWorld || !SoundMix || !SoundClass) return;

	UGameplayStatics::SetSoundMixClassOverride(AudioWorld, SoundMix, SoundClass, Volume, 1.f, 0.2f, bApplyToChildren);
}

void USL_GameUserSettings::SetOverallVolume(float InVolume)
{
	OverallVolume = InVolume;
	ApplySoundMixVolumes();
}

void USL_GameUserSettings::SetMusicVolume(float InVolume)
{
	MusicVolume = InVolume;
	ApplySoundMixVolumes();
}

void USL_GameUserSettings::SetSoundFXVolume(float InVolume)
{
	SoundFXVolume = InVolume;
	ApplySoundMixVolumes();
}

void USL_GameUserSettings::SetAllowBackgroundAudio(bool bIsAllowed)
{
	bAllowBackgroundAudio = bIsAllowed;
	ApplyAllowBackgroundAudio();
}

void USL_GameUserSettings::SetUseHDRAudioMode(bool bIsAllowed)
{
	bUseHDRAudioMode = bIsAllowed;
	ApplyHDRAudioMode();
}