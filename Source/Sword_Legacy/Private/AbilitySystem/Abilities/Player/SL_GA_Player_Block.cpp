#include "AbilitySystem/Abilities/Player/SL_GA_Player_Block.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Items/Weapons/SL_PlayerWeapon.h"
#include "Utilities/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Player_Block::USL_GA_Player_Block()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	SuccessfulBlockEventTag = SL_GameplayTags::Player_Event_SuccessfulBlock;
	SuccessfulBlockGameplayCueTag = SL_GameplayTags::GameplayCue_Effects_Katana_SuccessfulBlock;
}

void USL_GA_Player_Block::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	PlayerBlockActivatedTime = GetWorld()->GetTimeSeconds();
	bIsPerfectBlock = false;
	
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SuccessfulBlockEventTag,
		nullptr,
		false,
		true
	);
	
	if (WaitGameplayEventTask)
	{
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &USL_GA_Player_Block::OnSuccessfulBlockEventReceived);
		WaitGameplayEventTask->ReadyForActivation();
	}

	if (BlockMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			BlockMontage
		);
		
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_Block::OnBlockFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_Block::OnBlockFinished);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USL_GA_Player_Block::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) != 1.0f)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeDilationTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(ResetJumpToFinisherTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FGameplayCueParameters USL_GA_Player_Block::MakeBlockGameplayCueParams() const
{
	FGameplayCueParameters Params;
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		Params.TargetAttachComponent = ActorInfo->SkeletalMeshComponent.Get();
	}
	return Params;
}

void USL_GA_Player_Block::OnSuccessfulBlockEventReceived(FGameplayEventData Payload)
{
	AActor* Attacker = const_cast<AActor*>(Payload.Instigator.Get());
	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	
	if (!PlayerCharacter || !Attacker) return;
	
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	bIsPerfectBlock = (CurrentTime - PlayerBlockActivatedTime) <= PerfectBlockTimeWindow;

	const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		PlayerCharacter->GetActorLocation(), 
		Attacker->GetActorLocation()
	);
	
	const FRotator NewRot = FRotator(0.f, LookAtRot.Yaw, 0.f);
	PlayerCharacter->SetActorRotation(NewRot);

	UAbilityTask_ApplyRootMotionConstantForce* ApplyRootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		PlayerCharacter->GetActorForwardVector() * -1.f,
		KnockbackStrength,
		KnockbackDuration,
		true,
		nullptr,
		ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
		FVector::ZeroVector,
		0.f,
		true
	);
	
	if (ApplyRootMotionTask)
	{
		ApplyRootMotionTask->ReadyForActivation();
	}

	if (SuccessfulBlockGameplayCueTag.IsValid())
	{
		FGameplayCueParameters CueParams = MakeBlockGameplayCueParams();
		K2_ExecuteGameplayCueWithParams(SuccessfulBlockGameplayCueTag, CueParams);
	}

	if (bIsPerfectBlock)
	{
		USL_FunctionLibrary::AddGameplayTagToActorIfNone(PlayerCharacter, SL_GameplayTags::Player_Status_JumpToFinisher);

		FGameplayCueParameters PerfectCueParams = MakeBlockGameplayCueParams();
		K2_ExecuteGameplayCueWithParams(SL_GameplayTags::GameplayCue_Effects_Katana_PerfectBlock, PerfectCueParams);

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), PerfectBlockTimeDilation);

		GetWorld()->GetTimerManager().SetTimer(
			TimeDilationTimerHandle,
			this,
			&USL_GA_Player_Block::RestoreTimeDilation,
			PerfectBlockDilationDuration,
			false
		);
	}
}

void USL_GA_Player_Block::RestoreTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	StartResetJumpToFinisherTimer();
}

void USL_GA_Player_Block::StartResetJumpToFinisherTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		ResetJumpToFinisherTimerHandle,
		this,
		&USL_GA_Player_Block::ResetJumpToFinisherState,
		JumpToFinisherTagDuration,
		false
	);
}

void USL_GA_Player_Block::ResetJumpToFinisherState()
{
	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (PlayerCharacter)
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(PlayerCharacter, SL_GameplayTags::Player_Status_JumpToFinisher);
	}
}

void USL_GA_Player_Block::OnBlockFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}