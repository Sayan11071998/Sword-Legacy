#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_SideDodge.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "TimerManager.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Enemy_SideDodge::USL_GA_Enemy_SideDodge()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(SL_GameplayTags::Enemy_Ability_SideDodge);
	ActivationOwnedTags.AddTag(SL_GameplayTags::Enemy_Status_Dodging);
	ActivationBlockedTags.AddTag(SL_GameplayTags::Enemy_Cooldown_Dodge);
}

void USL_GA_Enemy_SideDodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bDodgeFinished = false;

	if (!DodgeMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		DodgeMontage
	);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_SideDodge::OnDodgeFinished);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_SideDodge::OnDodgeFinished);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_SideDodge::OnDodgeFinished);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_SideDodge::OnDodgeFinished);
	PlayMontageTask->ReadyForActivation();
}

void USL_GA_Enemy_SideDodge::OnDodgeFinished()
{
	if (bDodgeFinished) return;

	bDodgeFinished = true;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Enemy_SideDodge::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bDodgeFinished && DodgeCooldown > 0.f)
	{
		if (AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			USL_FunctionLibrary::AddGameplayTagToActorIfNone(Avatar, SL_GameplayTags::Enemy_Cooldown_Dodge);
		}

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(DodgeCooldownTimer, this, &USL_GA_Enemy_SideDodge::ClearDodgeCooldown, DodgeCooldown, false);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Enemy_SideDodge::ClearDodgeCooldown()
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, SL_GameplayTags::Enemy_Cooldown_Dodge);
	}
}