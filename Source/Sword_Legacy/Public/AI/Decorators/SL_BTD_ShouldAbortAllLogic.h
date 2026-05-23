#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SL_BTD_ShouldAbortAllLogic.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BTD_ShouldAbortAllLogic : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	USL_BTD_ShouldAbortAllLogic();
	
	// ~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface
	
protected:
	// ~ Begin UBTDecorator Interface
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	// ~ End UBTDecorator Interface
	
private:
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector InTargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector InDistToTargetKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FString CustomDescription;
};