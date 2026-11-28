#include "AI/Tasks/SL_BTT_WaitWhileBlocking.h"
#include "AIController.h"
#include "Characters/SL_EnemyCharacter.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_BTT_WaitWhileBlocking::USL_BTT_WaitWhileBlocking()
{
	NodeName = TEXT("Wait While Blocking");

	bNotifyTick = true;

	INIT_TASK_NODE_NOTIFY_FLAGS();
}

FString USL_BTT_WaitWhileBlocking::GetStaticDescription() const
{
	return TEXT("Waits until Enemy.Status.Blocking is removed");
}

EBTNodeResult::Type USL_BTT_WaitWhileBlocking::OnEnemyExecuteTask(AAIController* OwnerController, ASL_EnemyCharacter* OwningEnemyCharacter)
{
	if (!USL_FunctionLibrary::NativeDoesActorHaveTag(OwningEnemyCharacter, SL_GameplayTags::Enemy_Status_Blocking))
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

void USL_BTT_WaitWhileBlocking::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* OwnerController = OwnerComp.GetAIOwner();

	ASL_EnemyCharacter* EnemyCharacter = OwnerController ? Cast<ASL_EnemyCharacter>(OwnerController->GetPawn()) : nullptr;

	if (!EnemyCharacter || !USL_FunctionLibrary::NativeDoesActorHaveTag(EnemyCharacter, SL_GameplayTags::Enemy_Status_Blocking))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}