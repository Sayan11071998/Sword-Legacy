#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_HitReactBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Utilities/SL_GameplayTags.h"

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
	
	if (bFaceAttacker && TriggerEventData && TriggerEventData->Instigator)
	{
		if (ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo())
		{
			const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
				EnemyCharacter->GetActorLocation(), 
				TriggerEventData->Instigator->GetActorLocation()
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
	
	if (!MontagesToPlay.IsEmpty())
	{
		const int32 RandomIndex = FMath::RandRange(0, MontagesToPlay.Num() - 1);
		
		if (UAnimMontage* SelectedMontage = MontagesToPlay[RandomIndex])
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
			
			return;
		}
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