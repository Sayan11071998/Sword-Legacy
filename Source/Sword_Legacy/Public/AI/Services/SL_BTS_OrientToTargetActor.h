#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SL_BTS_OrientToTargetActor.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BTS_OrientToTargetActor : public UBTService
{
	GENERATED_BODY()
	
public:
	USL_BTS_OrientToTargetActor();
	
	// ~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface
	
protected:
	// ~ Begin UBTService Interface
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// ~ End UBTService Interface
	
private:
	UPROPERTY(EditAnywhere, Category = "Target")
	FBlackboardKeySelector InTargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Target")
	float RotationInterpSpeed;
};