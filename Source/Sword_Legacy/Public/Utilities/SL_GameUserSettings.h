#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SL_GameUserSettings.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_GameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	static TObjectPtr<USL_GameUserSettings> Get();

	// ~ Begin UGameUserSettings Interface
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	// ~ End UGameUserSettings Interface
	
	// Easy = 1, Normal = 2, Hard = 3, Very Hard = 4. Passed as GAS ApplyLevel for player and enemy startup data.
	int32 GetCurrentGameDifficultyAsAbilityLevel() const;

	void ApplyCurrentGameDifficultyToAbilitySystems() const;
	
	// Gameplay Collection Tab
	UFUNCTION()
	FString GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
	
	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty);

private:
	UPROPERTY(Config)
	FString CurrentGameDifficulty = TEXT("Normal");
};