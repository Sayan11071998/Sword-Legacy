#include "Components/Combat/SL_EnemyCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Utilities/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

void USL_EnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor)) return;
	
	OverlappedActors.AddUnique(HitActor);
	
	bool bIsValidBlock = false;
	
	const bool bIsPlayerBlocking = USL_FunctionLibrary::NativeDoesActorHaveTag(HitActor, SL_GameplayTags::Player_Status_Blocking);
	const bool bIsMyAttackUnblockable = USL_FunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(), SL_GameplayTags::Enemy_Status_Unblockable);
	
	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		bIsValidBlock = USL_FunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	
	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			SL_GameplayTags::Player_Event_SuccessfulBlock,
			EventData
		);
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