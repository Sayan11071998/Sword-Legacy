#include "AbilitySystem/Abilities/Player/SL_GA_Player_PickupStones.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Items/Collectables/Stones/SL_StoneBase.h"

USL_GA_Player_PickupStones::USL_GA_Player_PickupStones()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Player_PickupStones::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USL_GA_Player_PickupStones::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Player_PickupStones::CollectStones()
{
	CollectedStones.Empty();
	
	TArray<FHitResult> TraceHits;
	
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetPlayerCharacterFromActorInfo(),
		GetPlayerCharacterFromActorInfo()->GetActorLocation(),
		GetPlayerCharacterFromActorInfo()->GetActorLocation() + (-GetPlayerCharacterFromActorInfo()->GetActorUpVector()) * BoxTraceDistance,
		TraceBoxSize / 2.f,
		(-GetPlayerCharacterFromActorInfo()->GetActorUpVector()).ToOrientationRotator(),
		StoneTraceChannel,
		false,
		TArray<AActor*>(),
		bDrawPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		TraceHits,
		true
	);
	
	for (const FHitResult& TraceHit : TraceHits)
	{
		if (ASL_StoneBase* FoundStone = Cast<ASL_StoneBase>(TraceHit.GetActor()))
		{
			CollectedStones.AddUnique(FoundStone);
		}
	}
	
	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
}