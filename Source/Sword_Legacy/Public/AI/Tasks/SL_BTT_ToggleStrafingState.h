#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/SL_BTT_EnemyBase.h"
#include "SL_BTT_ToggleStrafingState.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BTT_ToggleStrafingState : public USL_BTT_EnemyBase
{
	GENERATED_BODY()
	
public:
	USL_BTT_ToggleStrafingState();
	
	// ~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface
	
protected:
	// ~ Begin USL_BTT_EnemyBase Interface
	virtual EBTNodeResult::Type OnEnemyExecuteTask(AAIController* OwnerController, ASL_EnemyCharacter* OwningEnemyCharacter) override;
	// ~ End USL_BTT_EnemyBase Interface
	
private:
	UPROPERTY(EditAnywhere, Category = "Strafing")
	bool bShouldEnable;
	
	UPROPERTY(EditAnywhere, Category = "Strafing")
	bool bShouldChangeMaxWalkSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Strafing")
	float StrafingWalkSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Strafing")
	FBlackboardKeySelector InDefaultMaxWalkSpeedKey;
};