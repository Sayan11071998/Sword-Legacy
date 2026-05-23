#include "Components/Combat/SL_EnemyCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Utilities/SL_GameplayTags.h"

void USL_EnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor)) return;
	
	OverlappedActors.AddUnique(HitActor);
	
	bool bIsValidBlock = false;
	
	const bool bIsPlayerBlocking = false;
	const bool bIsMyAttackUnblockable = false;
	
	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	
	if (bIsValidBlock)
	{
		
	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			SL_GameplayTags::Shared_Event_MeleeHit,
			EventData
		);
	}
}