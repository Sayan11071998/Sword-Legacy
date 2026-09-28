#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

class USoundClass;
class USoundMix;
class UWorld;

#include "SL_GameUserSettings.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	USL_GameUserSettings();
	
	static TObjectPtr<USL_GameUserSettings> Get();

	// ~ Begin UGameUserSettings Interface
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	// ~ End UGameUserSettings Interface
	
	// Easy = 1, Normal = 2, Hard = 3, Very Hard = 4. Passed as GAS ApplyLevel for player and enemy startup data.
	int32 GetCurrentGameDifficultyAsAbilityLevel() const;

	void ApplyCurrentGameDifficultyToAbilitySystems() const;
	
	void ApplySoundMixVolumes() const;
	
	// Gameplay Collection Tab
	UFUNCTION()
	FString GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
	
	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty);
	
	// Audio Collection Tab
	UFUNCTION()
	float GetOverallVolume() const { return OverallVolume; }
	
	UFUNCTION()
	void SetOverallVolume(float InVolume);
	
	UFUNCTION()
	float GetMusicVolume() const { return MusicVolume; }
	
	UFUNCTION()
	void SetMusicVolume(float InVolume);
	
	UFUNCTION()
	float GetSoundFXVolume() const { return SoundFXVolume; }
	
	UFUNCTION()
	void SetSoundFXVolume(float InVolume);
	
	UFUNCTION()
	bool GetAllowBackgroundAudio() const { return bAllowBackgroundAudio; }
	
	UFUNCTION()
	void SetAllowBackgroundAudio(bool bIsAllowed);
	
	UFUNCTION()
	bool GetUseHDRAudioMode() const { return bUseHDRAudioMode; }
	
	UFUNCTION()
	void SetUseHDRAudioMode(bool bIsAllowed);

private:
	UWorld* FindGameAudioWorld() const;
	
	void SetSoundClassVolume(
		UWorld* AudioWorld,
		USoundMix* SoundMix,
		USoundClass* SoundClass,
		float Volume,
		bool bApplyToChildren	
	) const;

	// Gameplay Collection Tab
	UPROPERTY(Config)
	FString CurrentGameDifficulty = TEXT("Normal");
	
	// Audio Collection Tab
	UPROPERTY(Config)
	float OverallVolume;
	
	UPROPERTY(Config)
	float MusicVolume;
	
	UPROPERTY(Config)
	float SoundFXVolume;
	
	UPROPERTY(Config)
	bool bAllowBackgroundAudio;
	
	UPROPERTY(Config)
	bool bUseHDRAudioMode;
};