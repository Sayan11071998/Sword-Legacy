#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SL_AIController.generated.h"

struct FAIStimulus;
class UAISenseConfig_Sight;

UCLASS()
class SWORD_LEGACY_API ASL_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASL_AIController(const FObjectInitializer& ObjectInitializer);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> EnemyPerceptionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAISenseConfig_Sight> AISenseConfig_Sight;
	
	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};