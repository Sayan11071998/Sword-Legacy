#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SL_BTD_ComputeSuccessChance.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BTD_ComputeSuccessChance : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	USL_BTD_ComputeSuccessChance();
	
	// ~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface
	
protected:
	// ~ Begin UBTDecorator Interface
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	// ~ End UBTDecorator Interface
	
private:
	UPROPERTY(EditAnywhere, Category = "Success Chance", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SuccessChanceMin = 0.65f;
	
	UPROPERTY(EditAnywhere, Category = "Success Chance", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SuccessChanceMax = 0.75f;
};