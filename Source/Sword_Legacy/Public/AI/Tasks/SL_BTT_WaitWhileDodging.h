#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/SL_BTT_EnemyBase.h"
#include "SL_BTT_WaitWhileDodging.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BTT_WaitWhileDodging : public USL_BTT_EnemyBase
{
	GENERATED_BODY()

public:
	USL_BTT_WaitWhileDodging();

	// ~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface

protected:
	// ~ Begin USL_BTT_EnemyBase Interface
	virtual EBTNodeResult::Type OnEnemyExecuteTask(AAIController* OwnerController, ASL_EnemyCharacter* OwningEnemyCharacter) override;
	// ~ End USL_BTT_EnemyBase Interface

	// ~ Begin UBTTaskNode Interface
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// ~ End UBTTaskNode Interface
};