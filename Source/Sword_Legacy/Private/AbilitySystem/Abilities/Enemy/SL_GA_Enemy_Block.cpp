#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_Block.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "TimerManager.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Enemy_Block::USL_GA_Enemy_Block()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(SL_GameplayTags::Enemy_Ability_Block);

	ActivationOwnedTags.AddTag(SL_GameplayTags::Enemy_Status_Blocking);

	SuccessfulBlockCueTag = SL_GameplayTags::GameplayCue_Effects_Katana_SuccessfulBlock;
}

void USL_GA_Enemy_Block::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bBlockFinished = false;

	UAbilityTask_WaitGameplayEvent* WaitBlockEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Enemy_Event_SuccessfulBlock,
		nullptr,
		false,
		true
	);
	WaitBlockEvent->EventReceived.AddDynamic(this, &USL_GA_Enemy_Block::OnSuccessfulBlock);
	WaitBlockEvent->ReadyForActivation();

	if (BlockHoldDuration <= 0.f)
	{
		FinishBlock();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(BlockHoldTimer, this, &USL_GA_Enemy_Block::OnBlockHoldFinished, BlockHoldDuration, false);
	}

	PlayBlockMontage();
}

void USL_GA_Enemy_Block::PlayBlockMontage()
{
	if (bBlockFinished || !BlockMontage) return;

	if (MontageTask && MontageTask->IsActive())
	{
		bRestartingMontage = true;
		MontageTask->EndTask();
		bRestartingMontage = false;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		BlockMontage
	);
	MontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_Block::OnBlockMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_Block::OnBlockMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_Block::OnBlockMontageCancelled);
	MontageTask->ReadyForActivation();

	LoopHoldSection();
}

void USL_GA_Enemy_Block::LoopHoldSection()
{
	if (!BlockMontage || HoldSectionName.IsNone() || !BlockMontage->IsValidSectionName(HoldSectionName)) return;

	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance())
		{
			AnimInstance->Montage_SetNextSection(HoldSectionName, HoldSectionName, BlockMontage);
		}
	}
}

void USL_GA_Enemy_Block::OnBlockMontageCompleted()
{
	MontageTask = nullptr;

	if (IsActive() && !bBlockFinished)
	{
		PlayBlockMontage();
	}
}

void USL_GA_Enemy_Block::OnBlockMontageCancelled()
{
	MontageTask = nullptr;

	if (bRestartingMontage) return;

	FinishBlock();
}

void USL_GA_Enemy_Block::OnSuccessfulBlock(FGameplayEventData Payload)
{
	if (!SuccessfulBlockCueTag.IsValid()) return;

	FGameplayCueParameters CueParams;
	CueParams.Instigator = const_cast<AActor*>(Payload.Instigator.Get());
	CueParams.SourceObject = GetAvatarActorFromActorInfo();

	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		CueParams.TargetAttachComponent = ActorInfo->SkeletalMeshComponent.Get();
	}

	K2_ExecuteGameplayCueWithParams(SuccessfulBlockCueTag, CueParams);
}

void USL_GA_Enemy_Block::OnBlockHoldFinished()
{
	FinishBlock();
}

void USL_GA_Enemy_Block::FinishBlock()
{
	if (bBlockFinished) return;

	bBlockFinished = true;

	if (MontageTask && MontageTask->IsActive())
	{
		MontageTask->EndTask();
	}

	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.2f, BlockMontage);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Enemy_Block::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BlockHoldTimer);
	}

	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, SL_GameplayTags::Enemy_Status_IncomingAttack);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}