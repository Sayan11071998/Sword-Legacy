#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/SL_LoadingScreenInterface.h"
#include "SL_PlayerController.generated.h"

class USL_Widget_PrimaryLayout;

UCLASS()
class SWORD_LEGACY_API ASL_PlayerController : public APlayerController, public IGenericTeamAgentInterface, public ISL_LoadingScreenInterface
{
	GENERATED_BODY()
	
public:
	ASL_PlayerController();
	
	// ~ Begin IGenericTeamAgentInterface Interface
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~ End IGenericTeamAgentInterface Interface

	void ShowGameLoseScreen();
	
protected:
	// ~ Begin APlayerController Interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* aPawn) override;
	// ~ End APlayerController Interface

	// ~ Begin ISL_LoadingScreenInterface
	virtual void OnLoadingScreenDeactivated_Implementation() override;
	// ~ End ISL_LoadingScreenInterface

	void SetViewTargetToDefaultCamera();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USL_Widget_PrimaryLayout> PrimaryLayoutWidgetClass;
	
private:
	void SetupFrontendUI();
	void SetupGameplayUI();
	void EnsurePrimaryLayoutCreated();

	FGenericTeamId PlayerTeamID;
	
	bool bHasHandledLoadingScreenDeactivation = false;
};