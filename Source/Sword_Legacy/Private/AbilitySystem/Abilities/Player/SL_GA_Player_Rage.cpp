#include "AbilitySystem/Abilities/Player/SL_GA_Player_Rage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "AbilitySystemComponent.h"
#include "Utilities/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Characters/SL_PlayerCharacter.h"

USL_GA_Player_Rage::USL_GA_Player_Rage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bIgnoreFirstActivateEvent = false;
}

void USL_GA_Player_Rage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (!PlayerCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (TriggerEventData && TriggerEventData->EventTag == SL_GameplayTags::Player_Event_ActivateRage)
	{
		bIgnoreFirstActivateEvent = true;
	}
	else
	{
		bIgnoreFirstActivateEvent = false;
	}

	USL_FunctionLibrary::AddGameplayTagToActorIfNone(PlayerCharacter, SL_GameplayTags::Player_Status_Rage_Activating);

	if (!RageActivationMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		RageActivationMontage,
		1.0f,
		NAME_None,
		true
	);

	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_Rage::OnMontageFinished);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_Rage::OnMontageFinished);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_Rage::OnMontageFinished);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_Rage::OnMontageFinished);
	PlayMontageTask->ReadyForActivation();


	UAbilityTask_WaitGameplayEvent* WaitToggleOffTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Player_Event_ActivateRage,
		nullptr,
		true,
		true
	);
	WaitToggleOffTask->EventReceived.AddDynamic(this, &USL_GA_Player_Rage::OnActivateRageEventReceived);
	WaitToggleOffTask->ReadyForActivation();
	
	UAbilityTask_WaitGameplayTagAdded* WaitRageDepletedTask = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(
		this,
		SL_GameplayTags::Player_Status_Rage_None
	);
	WaitRageDepletedTask->Added.AddDynamic(this, &USL_GA_Player_Rage::OnRageNoneTagAdded);
	WaitRageDepletedTask->ReadyForActivation();
	
	if (RageCostEffectClass)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(RageCostEffectClass, GetAbilityLevel());
			if (SpecHandle.IsValid())
			{
				AppliedRageCostEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void USL_GA_Player_Rage::OnMontageFinished()
{
	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (PlayerCharacter)
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(PlayerCharacter, SL_GameplayTags::Player_Status_Rage_Activating);
	}
}

void USL_GA_Player_Rage::OnActivateRageEventReceived(FGameplayEventData Payload)
{
	if (bIgnoreFirstActivateEvent)
	{
		bIgnoreFirstActivateEvent = false;
		return;
	}

	FGameplayCueParameters CueParams;
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		CueParams.TargetAttachComponent = ActorInfo->SkeletalMeshComponent.Get();
	}
	
	K2_AddGameplayCueWithParams(
		SL_GameplayTags::GameplayCue_Effects_Rage_Activating,
		CueParams,
		true
	);
}

void USL_GA_Player_Rage::OnRageNoneTagAdded()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void USL_GA_Player_Rage::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AppliedRageCostEffectHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveActiveGameplayEffect(AppliedRageCostEffectHandle, -1);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}