#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SL_BTT_EnemyBase.generated.h"

class ASL_EnemyCharacter;

UCLASS()
class SWORD_LEGACY_API USL_BTT_EnemyBase : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	USL_BTT_EnemyBase();
	
	// ~ Begin USL_BTT_EnemyBase Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// ~ End USL_BTT_EnemyBase Interface
	
protected:
	virtual EBTNodeResult::Type OnEnemyExecuteTask(AAIController* OwnerController, ASL_EnemyCharacter* OwningEnemyCharacter);
};