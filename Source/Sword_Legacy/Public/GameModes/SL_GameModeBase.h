#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h"
#include "UObject/WeakObjectPtr.h"
#include "SL_GameModeBase.generated.h"

class ASL_EnemyCharacter;

/** Gameplay game mode: player gets game-only input with the cursor hidden. */
UCLASS()
class SWORD_LEGACY_API ASL_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Game")
	void HandlePlayerLost();

	void RegisterSummonedEnemy(ASL_EnemyCharacter* Enemy);
	void NotifyEnemyRosterDeath(ASL_EnemyCharacter* Enemy);

	bool HasPlayerLost() const { return bHasHandledPlayerLost; }
	bool HasLevelCleared() const { return bHasHandledLevelCleared; }

protected:
	// ~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~ End AActor Interface

	// ~ Begin AGameModeBase Interface
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	// ~ End AGameModeBase Interface

private:
	void BeginGameplay();
	
	void UnbindLoadingScreen();
	
	void BuildEnemyRoster();
	
	int32 CountPlacedEnemies() const;
	int32 GetEnemyRosterSize() const;
	
	bool IsAnyPlayerDead() const;
	
	void TryCompleteLevel();
	
	void HandleLevelCleared();

	void LockClearedLevel();

	void StartLevelTransitionFade();

	void FinishLevelClearPresentation();

	void LoadNextLevelAfterClear();

	void FadeLevelCamera(float FromAlpha, float ToAlpha, bool bHoldWhenFinished) const;

	TSet<TWeakObjectPtr<AActor>> SummonedEnemies;
	TSet<TWeakObjectPtr<AActor>> DefeatedEnemies;

	int32 PlacedEnemyCount = 0;
	int32 SummonedEnemyCount = 0;
	int32 DefeatedEnemyCount = 0;

	bool bEnemyRosterReady = false;
	bool bHasHandledPlayerLost = false;
	bool bHasHandledLevelCleared = false;
	bool bHasBegunGameplay = false;
	bool bAwaitingGameEnding = false;

	FGameplayTag PendingNextLevelTag;
	FTimerHandle LevelClearHoldTimer;
};