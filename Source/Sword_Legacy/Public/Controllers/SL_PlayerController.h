#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "SL_PlayerController.generated.h"

class USL_Widget_PrimaryLayout;

UCLASS()
class SWORD_LEGACY_API ASL_PlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ASL_PlayerController();
	
	// ~ Begin IGenericTeamAgentInterface Interface
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~ End IGenericTeamAgentInterface Interface
	
protected:
	// ~ Begin APlayerController Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* aPawn) override;
	// ~ End APlayerController Interface

	void SetViewTargetToDefaultCamera();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USL_Widget_PrimaryLayout> PrimaryLayoutWidgetClass;
	
private:
	FGenericTeamId PlayerTeamID;
};