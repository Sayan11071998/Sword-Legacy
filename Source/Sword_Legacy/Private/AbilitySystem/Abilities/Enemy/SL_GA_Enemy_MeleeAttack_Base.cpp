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
	SetupMeleeMotionWarp(GetBlackboardAttackTarget());

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

void USL_GA_Enemy_MeleeAttack_Base::SetupMeleeMotionWarp(AActor* Target)
{
	const ASL_BaseCharacter* Enemy = Cast<ASL_BaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Enemy || AttackWarpTargetName.IsNone()) return;

	UMotionWarpingComponent* MotionWarping = Enemy->GetMotionWarpingComponent();
	if (!MotionWarping) return;

	if (!IsTargetInWarpRange(Enemy, Target))
	{
		MotionWarping->RemoveWarpTarget(AttackWarpTargetName);
		return;
	}

	const FVector EnemyLocation = Enemy->GetActorLocation();
	FVector ToTarget = Target->GetActorLocation() - EnemyLocation;
	ToTarget.Z = 0.f;

	const FVector Direction = ToTarget.GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		MotionWarping->RemoveWarpTarget(AttackWarpTargetName);
		return;
	}

	FVector WarpLocation = Target->GetActorLocation() - Direction * AttackStopDistance;
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

bool USL_GA_Enemy_MeleeAttack_Base::IsTargetInWarpRange(const AActor* Enemy, const AActor* Target) const
{
	if (!Enemy || !Target || Target == Enemy) return false;

	if (FVector::DistSquared2D(Enemy->GetActorLocation(), Target->GetActorLocation()) > FMath::Square(AttackWarpDistance))
	{
		return false;
	}

	const FVector Forward = Enemy->GetActorForwardVector().GetSafeNormal2D();
	const FVector ToTarget = (Target->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal2D();
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(AttackWarpHalfAngle));
	return FVector::DotProduct(Forward, ToTarget) >= MinDot;
}