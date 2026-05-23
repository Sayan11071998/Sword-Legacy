#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SL_BTD_DoesActorHaveTag.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BTD_DoesActorHaveTag : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	USL_BTD_DoesActorHaveTag();
	
	// ~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface
	
protected:
	// ~ Begin UBTDecorator Interface
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	// ~ End UBTDecorator Interface
	
private:
	UPROPERTY(EditAnywhere, Category = "Gameplay Tag")
	FBlackboardKeySelector InActorKeyToCheck;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay Tag")
	FGameplayTag TagToCheck;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay Tag")
	bool bInverseConditionCheck;
};