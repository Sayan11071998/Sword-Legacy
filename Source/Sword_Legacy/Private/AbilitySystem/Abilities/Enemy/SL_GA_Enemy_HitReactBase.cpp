#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_HitReactBase.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/SL_EnemyCharacter.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Enemy_HitReactBase::USL_GA_Enemy_HitReactBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SL_GameplayTags::Shared_Event_HitReact;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USL_GA_Enemy_HitReactBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Attacker = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Instigator.Get()) : nullptr;
	UAnimMontage* SelectedMontage = SelectHitReactMontage(Attacker);

	if (bFaceAttacker && Attacker && !IsDirectionalHitReactMontage(SelectedMontage))
	{
		if (ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo())
		{
			const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
				EnemyCharacter->GetActorLocation(), 
				Attacker->GetActorLocation()
			);
			const FRotator NewRot = FRotator(0.f, LookAtRot.Yaw, 0.f);
			EnemyCharacter->SetActorRotation(NewRot);
		}
	}
	
	if (USkeletalMeshComponent* SkeletalMeshComponent = GetCurrentActorInfo()->SkeletalMeshComponent.Get())
	{
		SkeletalMeshComponent->SetScalarParameterValueOnMaterials(HitParameterName, HitFlashIntensity);
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
	
	if (SelectedMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			SelectedMontage
		);
		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_HitReactBase::OnHitReactFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_HitReactBase::OnHitReactFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_HitReactBase::OnHitReactFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_HitReactBase::OnHitReactFinished);
		PlayMontageTask->ReadyForActivation();
		ApplyHitReactPushback(Attacker);
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		HitReactFallbackTimerHandle,
		this,
		&USL_GA_Enemy_HitReactBase::OnHitReactFinished,
		HitReactFallbackDuration,
		false
	);
}

void USL_GA_Enemy_HitReactBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitReactFallbackTimerHandle);
	}
	
	if (USkeletalMeshComponent* SkeletalMeshComponent = GetCurrentActorInfo()->SkeletalMeshComponent.Get())
	{
		SkeletalMeshComponent->SetScalarParameterValueOnMaterials(HitParameterName, 0.f);
	}
	
	if (AppliedHitReactEffectHandle.IsValid())
	{
		GetPawnAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(AppliedHitReactEffectHandle);
		AppliedHitReactEffectHandle.Invalidate();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Enemy_HitReactBase::OnHitReactFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

UAnimMontage* USL_GA_Enemy_HitReactBase::SelectHitReactMontage(AActor* Attacker) const
{
	AActor* Victim = GetAvatarActorFromActorInfo();

	if (Attacker && Victim)
	{
		float AngleDifference = 0.f;

		const FGameplayTag DirectionTag = USL_FunctionLibrary::ComputeHitReactDirectionTag(Attacker, Victim, AngleDifference);

		UAnimMontage* DirectionMontage = FrontHitReactMontage;

		if (DirectionTag.MatchesTagExact(SL_GameplayTags::Shared_Status_HitReact_Left))
		{
			DirectionMontage = LeftHitReactMontage;
		}
		else if (DirectionTag.MatchesTagExact(SL_GameplayTags::Shared_Status_HitReact_Right))
		{
			DirectionMontage = RightHitReactMontage;
		}
		else if (DirectionTag.MatchesTagExact(SL_GameplayTags::Shared_Status_HitReact_Back))
		{
			DirectionMontage = BackHitReactMontage;
		}

		if (DirectionMontage)
		{
			return DirectionMontage;
		}

		if (FrontHitReactMontage)
		{
			return FrontHitReactMontage;
		}
	}

	return nullptr;
}

bool USL_GA_Enemy_HitReactBase::IsDirectionalHitReactMontage(const UAnimMontage* Montage) const
{
	return Montage && (Montage == FrontHitReactMontage || Montage == LeftHitReactMontage
		|| Montage == RightHitReactMontage || Montage == BackHitReactMontage);
}

void USL_GA_Enemy_HitReactBase::ApplyHitReactPushback(AActor* Attacker)
{
	if (HitReactPushbackStrength <= 0.f || HitReactPushbackDuration <= 0.f) return;

	AActor* Victim = GetAvatarActorFromActorInfo();
	if (!Victim) return;

	FVector PushDirection = Attacker
		? Victim->GetActorLocation() - Attacker->GetActorLocation()
		: -Victim->GetActorForwardVector();
	
	PushDirection.Z = 0.f;
	PushDirection = PushDirection.GetSafeNormal();
	
	if (PushDirection.IsNearlyZero())
	{
		PushDirection = -Victim->GetActorForwardVector();
	}

	UAbilityTask_ApplyRootMotionConstantForce* ApplyRootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		PushDirection,
		HitReactPushbackStrength,
		HitReactPushbackDuration,
		true,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		0.f,
		true
	);

	if (ApplyRootMotionTask)
	{
		ApplyRootMotionTask->ReadyForActivation();
	}
}