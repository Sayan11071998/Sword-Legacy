#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_UnblockableHitReact.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Characters/SL_EnemyCharacter.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

USL_GA_Enemy_UnblockableHitReact::USL_GA_Enemy_UnblockableHitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SL_GameplayTags::Shared_Event_UnblockableHitReact;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

	AbilityTriggers.Add(TriggerData);

	CancelAbilitiesWithTag.AddTag(SL_GameplayTags::Enemy_Ability_Block);
}

void USL_GA_Enemy_UnblockableHitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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

	if (!HitParameterName.IsNone())
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = GetCurrentActorInfo()->SkeletalMeshComponent.Get())
		{
			SkeletalMeshComponent->SetScalarParameterValueOnMaterials(HitParameterName, HitFlashIntensity);
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

	if (UnblockableHitReactMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			UnblockableHitReactMontage
		);
		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_UnblockableHitReact::OnHitReactFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_UnblockableHitReact::OnHitReactFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_UnblockableHitReact::OnHitReactFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_UnblockableHitReact::OnHitReactFinished);
		PlayMontageTask->ReadyForActivation();
		
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			HitReactFallbackTimerHandle,
			this,
			&USL_GA_Enemy_UnblockableHitReact::OnHitReactFinished,
			HitReactFallbackDuration,
			false
		);
	}
}

void USL_GA_Enemy_UnblockableHitReact::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitReactFallbackTimerHandle);
	}

	if (!HitParameterName.IsNone())
	{
		if (const FGameplayAbilityActorInfo* CurrentInfo = GetCurrentActorInfo())
		{
			if (USkeletalMeshComponent* SkeletalMeshComponent = CurrentInfo->SkeletalMeshComponent.Get())
			{
				SkeletalMeshComponent->SetScalarParameterValueOnMaterials(HitParameterName, 0.f);
			}
		}
	}

	if (AppliedHitReactEffectHandle.IsValid())
	{
		GetPawnAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(AppliedHitReactEffectHandle);

		AppliedHitReactEffectHandle.Invalidate();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Enemy_UnblockableHitReact::OnHitReactFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}