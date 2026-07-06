#include "AI/Tasks/SL_BTT_EnemyBase.h"
#include "AIController.h"
#include "Characters/SL_EnemyCharacter.h"

USL_BTT_EnemyBase::USL_BTT_EnemyBase()
{
	NodeName = TEXT("Enemy Base Task");
}

EBTNodeResult::Type USL_BTT_EnemyBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController) return EBTNodeResult::Failed;

	ASL_EnemyCharacter* OwningEnemyCharacter = Cast<ASL_EnemyCharacter>(OwnerController->GetPawn());
	if (!OwningEnemyCharacter) return EBTNodeResult::Failed;

	return OnEnemyExecuteTask(OwnerController, OwningEnemyCharacter);
}

EBTNodeResult::Type USL_BTT_EnemyBase::OnEnemyExecuteTask(AAIController* OwnerController,
	ASL_EnemyCharacter* OwningEnemyCharacter)
{
	return EBTNodeResult::Succeeded;
}