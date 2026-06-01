#include "AbilitySystem/Abilities/Player/SL_GA_Player_HitReact.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"

USL_GA_Player_HitReact::USL_GA_Player_HitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SL_GameplayTags::Shared_Event_HitReact;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USL_GA_Player_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* Attacker = const_cast<AActor*>(TriggerEventData->Instigator.Get());
	if (!Attacker)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ASL_PlayerCharacter* Victim = GetPlayerCharacterFromActorInfo();
	if (!Victim)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	float AngleDifference = 0.f;
	FGameplayTag DirectionTag = USL_FunctionLibrary::ComputeHitReactDirectionTag(Attacker, Victim, AngleDifference);
	
	if (BloodSplashFX)
	{
		FVector Direction = (Attacker->GetActorLocation() - Victim->GetActorLocation()).GetSafeNormal();
		FVector ImpactPoint = Victim->GetActorLocation();
		
		if (UCapsuleComponent* Capsule = Victim->GetCapsuleComponent())
		{
			ImpactPoint += Direction * Capsule->GetScaledCapsuleRadius();
		}
		else
		{
			ImpactPoint += Direction * 40.f;
		}
		ImpactPoint.Z += 50.f;

		FRotator ImpactRotation = (-Direction).Rotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BloodSplashFX,
			ImpactPoint,
			ImpactRotation
		);
	}

	UAnimMontage* SelectedMontage = nullptr;
	
	if (DirectionTag.MatchesTagExact(SL_GameplayTags::Shared_Status_HitReact_Front))
	{
		SelectedMontage = FrontHitReactMontage;
	}
	else if (DirectionTag.MatchesTagExact(SL_GameplayTags::Shared_Status_HitReact_Left))
	{
		SelectedMontage = LeftHitReactMontage;
	}
	else if (DirectionTag.MatchesTagExact(SL_GameplayTags::Shared_Status_HitReact_Right))
	{
		SelectedMontage = RightHitReactMontage;
	}
	else if (DirectionTag.MatchesTagExact(SL_GameplayTags::Shared_Status_HitReact_Back))
	{
		SelectedMontage = BackHitReactMontage;
	}
	else
	{
		SelectedMontage = FrontHitReactMontage;
	}
	
	if (SelectedMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			SelectedMontage
		);

		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_HitReact::OnHitReactFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_HitReact::OnHitReactFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_HitReact::OnHitReactFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_HitReact::OnHitReactFinished);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USL_GA_Player_HitReact::OnHitReactFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}