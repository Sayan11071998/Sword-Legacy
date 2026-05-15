#include "AI/EQS/QueryContext/SL_EQSContext_TargetActor.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

void USL_EQSContext_TargetActor::ProvideContext(FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	if (!QueryOwner) return;
	
	UBlackboardComponent* BlackboardComp = UAIBlueprintHelperLibrary::GetBlackboard(QueryOwner);
	if (!BlackboardComp) return;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(FName("TargetActor")));
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
}