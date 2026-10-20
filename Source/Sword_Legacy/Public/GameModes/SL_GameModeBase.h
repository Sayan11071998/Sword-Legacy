#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SL_GameModeBase.generated.h"

/** Gameplay game mode: player gets game-only input with the cursor hidden. */
UCLASS()
class SWORD_LEGACY_API ASL_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Game")
	void HandlePlayerLost();

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

	bool bHasHandledPlayerLost = false;
	bool bHasBegunGameplay = false;
};