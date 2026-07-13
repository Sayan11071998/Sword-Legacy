#include "GameModes/SL_MainMenuGameMode.h"
#include "Controllers/SL_PlayerController.h"

ASL_MainMenuGameMode::ASL_MainMenuGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ASL_PlayerController::StaticClass();
	bStartPlayersAsSpectators = true;
}