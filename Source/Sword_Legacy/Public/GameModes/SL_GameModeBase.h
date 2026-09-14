#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SL_GameModeBase.generated.h"

/** Gameplay game mode: player gets game-only input with the cursor hidden. */
UCLASS()
class SWORD_LEGACY_API ASL_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	// ~ Begin AGameModeBase Interface
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	// ~ End AGameModeBase Interface
};