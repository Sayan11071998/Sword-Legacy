#include "AbilitySystem/Abilities/Player/SL_GA_Player_Block.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Items/Weapons/SL_PlayerWeapon.h"
#include "Utilities/SL_GameplayTags.h"

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

void USL_GA_Player_Block::OnSuccessfulBlockEventReceived(FGameplayEventData Payload)
{
	AActor* Attacker = const_cast<AActor*>(Payload.Instigator.Get());
	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	
	if (!PlayerCharacter || !Attacker) return;

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
		FGameplayCueParameters CueParams;
		if (USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo())
		{
			if (ASL_PlayerWeapon* CurrentWeapon = CombatComponent->GetPlayerCurrentEquippedWeapon())
			{
				CueParams.TargetAttachComponent = CurrentWeapon->GetWeaponMesh();
			}
		}
		
		K2_ExecuteGameplayCueWithParams(SuccessfulBlockGameplayCueTag, CueParams);
	}
}

void USL_GA_Player_Block::OnBlockFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}