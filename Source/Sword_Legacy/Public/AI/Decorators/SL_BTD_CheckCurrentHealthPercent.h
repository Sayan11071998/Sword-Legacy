#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SL_BTD_CheckCurrentHealthPercent.generated.h"

UENUM(BlueprintType)
enum class ESL_DecoratorOperation : uint8
{
	IsLessThan               UMETA(DisplayName = "Is Less Than"),
	IsLessThanOrEqualTo      UMETA(DisplayName = "Is Less Than Or Equal To"),
	IsEqualTo                UMETA(DisplayName = "Is Equal To"),
	IsGreaterThanOrEqualTo   UMETA(DisplayName = "Is Greater Than Or Equal To"),
	IsGreaterThan            UMETA(DisplayName = "Is Greater Than")
};

UCLASS()
class SWORD_LEGACY_API USL_BTD_CheckCurrentHealthPercent : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	USL_BTD_CheckCurrentHealthPercent();
	
	// ~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface

protected:
	// ~ Begin UBTDecorator Interface
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	// ~ End UBTDecorator Interface

private:
	UPROPERTY(EditAnywhere, Category = "Condition")
	ESL_DecoratorOperation CheckOperation;
	
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float ThresholdToCheck;
};