#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_ParryRebound.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Enemy_ParryRebound::USL_GA_Enemy_ParryRebound()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(SL_GameplayTags::Enemy_Ability_ParryRebound);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SL_GameplayTags::Enemy_Event_ParryRebound;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	CancelAbilitiesWithTag.AddTag(SL_GameplayTags::Enemy_Ability_Melee);
	CancelAbilitiesWithTag.AddTag(SL_GameplayTags::Enemy_Ability_HeavyMelee);
	CancelAbilitiesWithTag.AddTag(SL_GameplayTags::Enemy_Ability_Combo);
	CancelAbilitiesWithTag.AddTag(SL_GameplayTags::Enemy_Ability_DashAttack);
}

void USL_GA_Enemy_ParryRebound::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* EnemyActor = GetAvatarActorFromActorInfo();

	if (EnemyActor)
	{
		USL_FunctionLibrary::AddGameplayTagToActorIfNone(EnemyActor, SL_GameplayTags::Enemy_Status_Rebounding);
		USL_FunctionLibrary::AddGameplayTagToActorIfNone(EnemyActor, SL_GameplayTags::Enemy_Status_UnderAttack);

		if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayTagContainer TagsToCancel;
			TagsToCancel.AddTag(SL_GameplayTags::Enemy_Ability_Melee);
			TagsToCancel.AddTag(SL_GameplayTags::Enemy_Ability_HeavyMelee);
			TagsToCancel.AddTag(SL_GameplayTags::Enemy_Ability_Combo);
			TagsToCancel.AddTag(SL_GameplayTags::Enemy_Ability_DashAttack);
			TagsToCancel.AddTag(SL_GameplayTags::Enemy_Ability_Block);
			TagsToCancel.AddTag(SL_GameplayTags::Enemy_Ability_SideDodge);
			AbilitySystem->CancelAbilities(&TagsToCancel, nullptr, this);
		}

		if (APawn* EnemyPawn = Cast<APawn>(EnemyActor))
		{
			if (AAIController* AIController = Cast<AAIController>(EnemyPawn->GetController()))
			{
				AIController->StopMovement();
			}
		}
	}

	if (ActorInfo && ActorInfo->SkeletalMeshComponent.IsValid())
	{
		if (UAnimInstance* AnimInstance = ActorInfo->SkeletalMeshComponent->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.f);
			AnimInstance->Montage_Stop(0.f);
		}
	}

	AActor* Attacker = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Instigator.Get()) : nullptr;

	if (Attacker)
	{
		if (ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo())
		{
			const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
				EnemyCharacter->GetActorLocation(),
				Attacker->GetActorLocation()
			);

			EnemyCharacter->SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));
		}
	}

	if (HitReactEffectClass)
	{
		FGameplayEffectContextHandle EffectContext = GetPawnAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

		FGameplayEffectSpecHandle SpecHandle = GetPawnAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
			HitReactEffectClass,
			GetAbilityLevel(),
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			AppliedHitReactEffectHandle = GetPawnAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	if (ReboundMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			ReboundMontage
		);
		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_ParryRebound::OnReboundFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_ParryRebound::OnReboundFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_ParryRebound::OnReboundFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_ParryRebound::OnReboundFinished);
		PlayMontageTask->ReadyForActivation();
		
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ReboundFallbackTimerHandle,
			this,
			&USL_GA_Enemy_ParryRebound::OnReboundFinished,
			ReboundFallbackDuration,
			false
		);
	}
}

void USL_GA_Enemy_ParryRebound::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ReboundFallbackTimerHandle);
	}

	if (AppliedHitReactEffectHandle.IsValid())
	{
		GetPawnAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(AppliedHitReactEffectHandle);
		AppliedHitReactEffectHandle.Invalidate();
	}

	if (AActor* EnemyActor = GetAvatarActorFromActorInfo())
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(EnemyActor, SL_GameplayTags::Enemy_Status_Rebounding);
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(EnemyActor, SL_GameplayTags::Enemy_Status_UnderAttack);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Enemy_ParryRebound::OnReboundFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}