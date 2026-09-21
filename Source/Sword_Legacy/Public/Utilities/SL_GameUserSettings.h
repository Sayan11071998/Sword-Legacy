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
	
	// Gameplay Collection Tab
	UFUNCTION()
	FString GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
	
	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty) { CurrentGameDifficulty = InNewDifficulty; }
	
private:
	UPROPERTY(Config)
	FString CurrentGameDifficulty;
};