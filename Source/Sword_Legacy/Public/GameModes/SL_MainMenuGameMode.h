#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SL_MainMenuGameMode.generated.h"

UCLASS()
class SWORD_LEGACY_API ASL_MainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASL_MainMenuGameMode();

protected:
	// ~ Begin AGameModeBase Interface
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	// ~ End AGameModeBase Interface
};