#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

class USoundClass;
class USoundMix;
class USubmixEffectDynamicsProcessorPreset;
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
	virtual void LoadSettings(bool bForceReload = false) override;
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	// ~ End UGameUserSettings Interface
	
	// Apply Game Difficulty - Easy = 1, Normal = 2, Hard = 3, Very Hard = 4. Passed as GAS ApplyLevel for player and enemy startup data.
	int32 GetCurrentGameDifficultyAsAbilityLevel() const;
	void ApplyCurrentGameDifficultyToAbilitySystems() const;
	
	// Apply Game Sound Settings
	void ApplySoundMixVolumes() const;
	void ApplyAllowBackgroundAudio() const;
	void ApplyHDRAudioMode();
	void ApplyCameraDistance() const;
	
	// Gameplay Collection Tab
	UFUNCTION()
	FString GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
	
	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty);

	UFUNCTION(BlueprintPure, Category = "Gameplay Settings")
	bool GetShowBloodEffects() const { return bShowBloodEffects; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay Settings")
	void SetShowBloodEffects(bool bIsShown);

	UFUNCTION(BlueprintPure, Category = "Gameplay Settings")
	bool GetEnableCameraShake() const { return bEnableCameraShake; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay Settings")
	void SetEnableCameraShake(bool bIsEnabled);

	UFUNCTION(BlueprintPure, Category = "Gameplay Settings")
	float GetCameraDistance() const { return CameraDistance; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay Settings")
	void SetCameraDistance(float InDistance);
	
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
	float GetDialogueVolume() const { return DialogueVolume; }

	UFUNCTION()
	void SetDialogueVolume(float InVolume);

	UFUNCTION()
	float GetUIVolume() const { return UIVolume; }

	UFUNCTION()
	void SetUIVolume(float InVolume);
	
	UFUNCTION()
	bool GetAllowBackgroundAudio() const { return bAllowBackgroundAudio; }
	
	UFUNCTION()
	void SetAllowBackgroundAudio(bool bIsAllowed);
	
	UFUNCTION()
	bool GetUseHDRAudioMode() const { return bUseHDRAudioMode; }
	
	UFUNCTION()
	void SetUseHDRAudioMode(bool bIsAllowed);
	
	// Video Collection Tab
	UFUNCTION()
	float GetCurrentDisplayGamma() const;
	
	UFUNCTION()
	void SetCurrentDisplayGamma(float InNewGamma);

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

	UPROPERTY(Config)
	bool bShowBloodEffects = true;

	UPROPERTY(Config)
	bool bEnableCameraShake = true;

	UPROPERTY(Config)
	float CameraDistance = 1.f;
	
	// Audio Collection Tab
	UPROPERTY(Config)
	float OverallVolume;
	
	UPROPERTY(Config)
	float MusicVolume;
	
	UPROPERTY(Config)
	float SoundFXVolume;

	UPROPERTY(Config)
	float DialogueVolume;

	UPROPERTY(Config)
	float UIVolume;
	
	UPROPERTY(Config)
	bool bAllowBackgroundAudio;
	
	UPROPERTY(Config)
	bool bUseHDRAudioMode;

	UPROPERTY(Transient)
	TObjectPtr<USubmixEffectDynamicsProcessorPreset> HDRAudioLimiter;

	UPROPERTY()
	bool bHDRAudioLimiterActive = false;
};