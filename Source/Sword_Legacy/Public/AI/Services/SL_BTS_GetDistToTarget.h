#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SL_BTS_GetDistToTarget.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BTS_GetDistToTarget : public UBTService
{
	GENERATED_BODY()
	
public:
	USL_BTS_GetDistToTarget();
	
protected:
	// ~ Begin UBTService Interface
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// ~ End UBTService Interface
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector DistanceToTargetKey;
};