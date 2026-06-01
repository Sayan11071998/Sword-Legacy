#include "AbilitySystem/Abilities/Player/SL_GA_Player_Death.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Controllers/SL_PlayerController.h"
#include "Components/CapsuleComponent.h"

USL_GA_Player_Death::USL_GA_Player_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void USL_GA_Player_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
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
			
			PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_Death::OnDeathFinished);
			PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_Death::OnDeathFinished);
			PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_Death::OnDeathFinished);
			PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_Death::OnDeathFinished);
			PlayMontageTask->ReadyForActivation();
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USL_GA_Player_Death::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (USkeletalMeshComponent* MeshComponent = ActorInfo->SkeletalMeshComponent.Get())
	{
		MeshComponent->bPauseAnims = true;
	}
	
	if (ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		if (UCapsuleComponent* CapsuleComp = PlayerCharacter->GetCapsuleComponent())
		{
			CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
	if (ASL_PlayerController* PlayerController = GetPlayerControllerFromActorInfo())
	{
		FInputModeUIOnly InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputModeData);
		PlayerController->SetShowMouseCursor(true);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Player_Death::OnDeathFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}