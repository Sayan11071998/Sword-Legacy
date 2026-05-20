#include "Components/Combat/SL_EnemyCombatComponent.h"

#include "SL_DebugHelper.h"

void USL_EnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (HitActor)
	{
		Debug::Print(GetOwningPawn()->GetActorNameOrLabel() + TEXT(" is hitting ") + HitActor->GetActorNameOrLabel());
	}
}