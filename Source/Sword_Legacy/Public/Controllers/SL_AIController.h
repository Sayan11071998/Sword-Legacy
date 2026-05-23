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
	
	// ~ Begin IGenericTeamAgentInterface Interface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	// ~ End IGenericTeamAgentInterface Interface
	
protected:
	virtual void BeginPlay() override;
	
	// ~ Begin AAIController Interface
	virtual void OnPossess(APawn* InPawn) override;
	// ~ End AAIController Interface
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> EnemyPerceptionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAISenseConfig_Sight> AISenseConfig_Sight;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeToRun;
	
	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config")
	bool bEnableDetourCrowdAvoidance = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance", UIMin = "1", UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;
	
	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
};