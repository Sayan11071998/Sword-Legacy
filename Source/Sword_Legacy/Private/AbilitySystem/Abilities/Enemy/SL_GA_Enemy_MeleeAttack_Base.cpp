#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_MeleeAttack_Base.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/SL_BaseCharacter.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "TimerManager.h"

USL_GA_Enemy_MeleeAttack_Base::USL_GA_Enemy_MeleeAttack_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationOwnedTags.AddTag(SL_GameplayTags::Enemy_Status_Attacking);
	UnblockableAttackWarningSpawnOffset = 100.0f;
}

void USL_GA_Enemy_MeleeAttack_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Shared_Event_MeleeHit,
		nullptr,
		false,
		true
	);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::HandleApplyDamage);
	WaitGameplayEventTask->ReadyForActivation();
	
	bool bIsUnblockable = false;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		bIsUnblockable = ASC->HasMatchingGameplayTag(SL_GameplayTags::Enemy_Status_Unblockable);
	}

	if (bIsUnblockable)
	{
		FGameplayCueParameters CueParams;
		CueParams.RawMagnitude = UnblockableAttackWarningSpawnOffset;
		K2_ExecuteGameplayCueWithParams(SL_GameplayTags::GameplayCue_Effects_UnblockableWarning, CueParams);

		UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, UnblockableWarningDelay);
		if (WaitDelayTask)
		{
			WaitDelayTask->OnFinish.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::PlayMeleeAttackMontage);
			WaitDelayTask->ReadyForActivation();
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
	}
	else
	{
		PlayMeleeAttackMontage();
	}
}

void USL_GA_Enemy_MeleeAttack_Base::PlayMeleeAttackMontage()
{
	ClearWarpModifiers();
	SetupMeleeMotionWarp(GetBlackboardAttackTarget());

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			MeleeWarpTimer,
			FTimerDelegate::CreateUObject(this, &USL_GA_Enemy_MeleeAttack_Base::RefreshMeleeMotionWarp),
			0.016f,
			true
		);
	}

	if (MontageToPlay)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			MontageToPlay
		);

		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USL_GA_Enemy_MeleeAttack_Base::HandleApplyDamage(FGameplayEventData Payload)
{
	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());
	if (!TargetActor) return;
	
	if (DamageEffectClass)
	{
		const FGameplayEffectSpecHandle DamageSpecHandle = MakeEnemyDamageEffectSpecHandle(DamageEffectClass, InDamageScalableFloat);
		if (DamageSpecHandle.IsValid())
		{
			const FActiveGameplayEffectHandle ActiveHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, DamageSpecHandle);
			
			if (!ActiveHandle.WasSuccessfullyApplied()) return;

			if (WeaponHitSoundGameplayCueTag.IsValid())
			{
				FGameplayEffectContextHandle ContextHandle = DamageSpecHandle.Data->GetContext();
				K2_ExecuteGameplayCue(WeaponHitSoundGameplayCueTag, ContextHandle);
			}
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				TargetActor,
				SL_GameplayTags::Shared_Event_HitReact,
				Payload
			);
		}
	}
}

void USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Enemy_MeleeAttack_Base::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MeleeWarpTimer);
	}

	ClearWarpModifiers();

	if (const ASL_BaseCharacter* Enemy = Cast<ASL_BaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UMotionWarpingComponent* MotionWarping = Enemy->GetMotionWarpingComponent())
		{
			MotionWarping->RemoveWarpTarget(AttackWarpTargetName);
		}
	}

	LastWarpDirection = FVector::ZeroVector;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Enemy_MeleeAttack_Base::RefreshMeleeMotionWarp()
{
	SetupMeleeMotionWarp(GetBlackboardAttackTarget());
}

void USL_GA_Enemy_MeleeAttack_Base::ClearWarpModifiers() const
{
	const ASL_BaseCharacter* Enemy = Cast<ASL_BaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Enemy) return;

	UMotionWarpingComponent* MotionWarping = Enemy->GetMotionWarpingComponent();
	if (!MotionWarping) return;

	for (URootMotionModifier* Modifier : MotionWarping->GetModifiers())
	{
		if (!Modifier) continue;

		Modifier->Animation = nullptr;
		Modifier->StartTime = -1.f;
		Modifier->EndTime = -1.f;
		Modifier->SetState(ERootMotionModifierState::MarkedForRemoval);
	}
}

void USL_GA_Enemy_MeleeAttack_Base::SetupMeleeMotionWarp(AActor* Target)
{
	const ASL_BaseCharacter* Enemy = Cast<ASL_BaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Enemy || AttackWarpTargetName.IsNone()) return;

	UMotionWarpingComponent* MotionWarping = Enemy->GetMotionWarpingComponent();
	if (!MotionWarping || !Target || Target == Enemy) return;

	const FVector EnemyLocation = Enemy->GetActorLocation();
	FVector ToTarget = Target->GetActorLocation() - EnemyLocation;
	ToTarget.Z = 0.f;

	FVector Direction = ToTarget.GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		Direction = LastWarpDirection;
	}
	else
	{
		LastWarpDirection = Direction;
	}

	if (Direction.IsNearlyZero()) return;

	const float Distance = ToTarget.Size();
	FVector WarpLocation;
	if (Distance > AttackStopDistance)
	{
		WarpLocation = Target->GetActorLocation() - Direction * AttackStopDistance;
	}
	else
	{
		const float LookDistance = FMath::Max(Distance, 30.f);
		WarpLocation = EnemyLocation + Direction * LookDistance;
	}

	WarpLocation.Z = EnemyLocation.Z;

	const FRotator WarpRotation(0.f, Direction.Rotation().Yaw, 0.f);
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(AttackWarpTargetName, WarpLocation, WarpRotation);
}

AActor* USL_GA_Enemy_MeleeAttack_Base::GetBlackboardAttackTarget() const
{
	const APawn* EnemyPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	const AAIController* AIController = EnemyPawn ? Cast<AAIController>(EnemyPawn->GetController()) : nullptr;
	const UBlackboardComponent* Blackboard = AIController ? AIController->GetBlackboardComponent() : nullptr;
	
	if (!Blackboard) return nullptr;

	return Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetActor")));
}