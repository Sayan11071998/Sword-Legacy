#include "AbilitySystem/Abilities/Player/SL_GA_Player_Block.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Characters/SL_EnemyCharacter.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Items/Weapons/SL_PlayerWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Player_Block::USL_GA_Player_Block()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationOwnedTags.AddTag(SL_GameplayTags::Player_Status_Blocking);
	
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
	bPlayingParry = false;
	bBlockEndHandled = false;
	bEndedByMontageInterrupt = false;
	
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

	if (BlockStartMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			BlockStartMontage
		);
		
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_Block::OnBlockFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_Block::OnBlockFinished);
		PlayMontageTask->ReadyForActivation();
	}
}

void USL_GA_Player_Block::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bBlockEndHandled)
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	bBlockEndHandled = true;
	const bool bPlayBlockEnd = !bEndedByMontageInterrupt && !bPlayingParry && BlockEndMontage;

	if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) != 1.0f)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeDilationTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(ResetJumpToFinisherTimerHandle);
	}

	UAnimInstance* AnimInstance = nullptr;
	if (const ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		if (USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh())
		{
			AnimInstance = Mesh->GetAnimInstance();
		}
	}

	if (bPlayBlockEnd && AnimInstance)
	{
		HoldBlockPoseForEndMontage();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bPlayBlockEnd && AnimInstance)
	{
		const float PlayedLength = AnimInstance->Montage_Play(BlockEndMontage, 1.f);
		if (PlayedLength <= 0.f)
		{
			ReleaseBlockEndTag();
		}
		else
		{
			FOnMontageBlendingOutStarted BlendOutDelegate;
			BlendOutDelegate.BindUObject(this, &USL_GA_Player_Block::OnBlockEndMontageBlendingOut);
			AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, BlockEndMontage);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &USL_GA_Player_Block::OnBlockEndMontageBlendingOut);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, BlockEndMontage);
		}
	}
}

void USL_GA_Player_Block::HoldBlockPoseForEndMontage()
{
	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();

	if (!PlayerCharacter) return;

	if (USL_AbilitySystemComponent* AbilitySystemComponent = PlayerCharacter->GetCharacterAbilitySystemComponent())
	{
		AbilitySystemComponent->AddLooseGameplayTag(SL_GameplayTags::Player_Status_Blocking);
		++BlockEndTagHolds;
	}
}

void USL_GA_Player_Block::ReleaseBlockEndTag()
{
	if (BlockEndTagHolds <= 0) return;

	--BlockEndTagHolds;

	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	
	if (!PlayerCharacter) return;

	if (USL_AbilitySystemComponent* AbilitySystemComponent = PlayerCharacter->GetCharacterAbilitySystemComponent())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(SL_GameplayTags::Player_Status_Blocking);
	}
}

void USL_GA_Player_Block::OnBlockEndMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	ReleaseBlockEndTag();
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

	if (bIsPerfectBlock)
	{
		bPlayingParry = true;

		FGameplayEventData ParryEvent;
		ParryEvent.Instigator = PlayerCharacter;
		ParryEvent.Target = Attacker;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter, SL_GameplayTags::Player_Event_Parry, ParryEvent);

		if (ASL_EnemyCharacter* EnemyCharacter = Cast<ASL_EnemyCharacter>(Attacker))
		{
			EnemyCharacter->ForceParryRebound(PlayerCharacter);
		}
	}

	UAbilityTask_ApplyRootMotionConstantForce* ApplyRootMotionTask = !bPlayingParry
		? UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
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
	)
		: nullptr;
	
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
	if (bPlayingParry || bBlockEndHandled) return;

	bEndedByMontageInterrupt = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}