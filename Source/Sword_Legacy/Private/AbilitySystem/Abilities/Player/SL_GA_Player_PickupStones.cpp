#include "AbilitySystem/Abilities/Player/SL_GA_Player_PickupStones.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/SL_PlayerCharacter.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Items/Collectables/Stones/SL_StoneBase.h"
#include "AbilitySystem/AbilityTasks/SL_AbilityTask_ExecuteTaskOnTick.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/UI/SL_PlayerUIComponent.h"
#include "Utilities/SL_GameplayTags.h"

USL_GA_Player_PickupStones::USL_GA_Player_PickupStones()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Player_PickupStones::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	GetPlayerUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(true);
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USL_AbilityTask_ExecuteTaskOnTick* TickTask = USL_AbilityTask_ExecuteTaskOnTick::ExecuteTaskOnTick(this);
	if (TickTask)
	{
		TickTask->OnAbilityTaskTick.AddDynamic(this, &USL_GA_Player_PickupStones::OnPickupStonesTick);
		TickTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Player_Event_ConsumeStones,
		nullptr,
		false,
		true
	);
	
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddDynamic(this, &USL_GA_Player_PickupStones::OnConsumeStonesEventReceived);
		WaitEventTask->ReadyForActivation();
	}
}

void USL_GA_Player_PickupStones::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	GetPlayerUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(false);
	
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
		bDrawDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
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

void USL_GA_Player_PickupStones::ConsumeStones()
{
	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}
	
	for (ASL_StoneBase* CollectedStone : CollectedStones)
	{
		if (CollectedStone)
		{
			CollectedStone->Consume(Cast<USL_AbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()), GetAbilityLevel());
		}
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USL_GA_Player_PickupStones::OnPickupStonesTick(float DeltaTime)
{
	CollectStones();
}

void USL_GA_Player_PickupStones::OnConsumeStonesEventReceived(FGameplayEventData Payload)
{
	ConsumeStones();
}