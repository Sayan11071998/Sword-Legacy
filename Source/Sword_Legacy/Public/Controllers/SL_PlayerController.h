#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "SL_PlayerController.generated.h"

UCLASS()
class SWORD_LEGACY_API ASL_PlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ASL_PlayerController();
	
	// ~ Begin IGenericTeamAgentInterface Interface
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~ End IGenericTeamAgentInterface Interface
	
private:
	FGenericTeamId PlayerTeamID;
};