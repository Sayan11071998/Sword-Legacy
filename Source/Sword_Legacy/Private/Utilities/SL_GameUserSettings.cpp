#include "Utilities/SL_GameUserSettings.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/UObjectIterator.h"

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