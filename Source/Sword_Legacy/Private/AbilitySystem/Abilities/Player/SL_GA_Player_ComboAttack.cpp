#include "AbilitySystem/Abilities/Player/SL_GA_Player_ComboAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Engine/World.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

void USL_GA_Player_ComboAttack::PrepareComboAttack()
{
	bComboWindowOpen = false;
	bComboInputBuffered = false;
	bComboChainRequested = false;
	bMontageEndHandled = false;
	PlayingMontage = nullptr;
	BufferedComboHandle = FGameplayAbilitySpecHandle();

	UnbindActiveMontageTask();

	if (const UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (USL_GA_Player_ComboAttack* ComboAttack = Cast<USL_GA_Player_ComboAttack>(AbilitySpec.GetPrimaryInstance()))
			{
				ComboAttack->ClearComboResetTimer();
			}
		}
	}
}

int32 USL_GA_Player_ComboAttack::GetComboCount() const
{
	const ASL_PlayerCharacter* Player = Cast<ASL_PlayerCharacter>(GetAvatarActorFromActorInfo());
	
	return Player ? Player->GetMeleeComboCount() : 1;
}

void USL_GA_Player_ComboAttack::SetComboCount(int32 NewComboCount)
{
	if (ASL_PlayerCharacter* Player = Cast<ASL_PlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		Player->SetMeleeComboCount(NewComboCount);
	}
}

void USL_GA_Player_ComboAttack::ClearComboResetTimer()
{
	if (!ComboCountResetTimerHandle.IsValid()) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ComboCountResetTimerHandle);
	}
}

void USL_GA_Player_ComboAttack::BindComboMontage(UAbilityTask_PlayMontageAndWait* MontageTask, UAnimMontage* Montage)
{
	PlayingMontage = Montage;
	ActiveMontageTask = MontageTask;

	if (!MontageTask) return;

	MontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_ComboAttack::OnComboMontageCompleted);
	MontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_ComboAttack::OnComboMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_ComboAttack::OnComboMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_ComboAttack::OnComboMontageCompleted);
}

void USL_GA_Player_ComboAttack::ResetComboCount()
{
	SetComboCount(1);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	if (AvatarActor)
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);
	}
}

void USL_GA_Player_ComboAttack::ReceiveComboInput(FGameplayAbilitySpecHandle NextHandle)
{
	if (!IsActive() || bComboChainRequested || !NextHandle.IsValid()) return;

	BufferedComboHandle = NextHandle;
	bComboInputBuffered = true;

	if (bComboWindowOpen)
	{
		RequestComboChain();
	}
}

void USL_GA_Player_ComboAttack::OpenComboWindow(UAnimMontage* SourceMontage)
{
	if (!IsActive() || bComboChainRequested || SourceMontage != PlayingMontage) return;

	bComboWindowOpen = true;

	if (bComboInputBuffered)
	{
		RequestComboChain();
	}
}

void USL_GA_Player_ComboAttack::CloseComboWindow(UAnimMontage* SourceMontage)
{
	if (SourceMontage != PlayingMontage) return;

	bComboWindowOpen = false;
}

void USL_GA_Player_ComboAttack::RequestComboChain()
{
	if (!IsActive() || bComboChainRequested) return;

	bComboChainRequested = true;
	bComboWindowOpen = false;
	bComboInputBuffered = false;
	bMontageEndHandled = true;

	UnbindActiveMontageTask();

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	UWorld* World = GetWorld();

	const FGameplayAbilitySpecHandle Handle = BufferedComboHandle.IsValid() ? BufferedComboHandle : CurrentSpecHandle;
	BufferedComboHandle = FGameplayAbilitySpecHandle();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	if (AbilitySystemComponent && AbilitySystemComponent->TryActivateAbility(Handle)) return;

	StartComboResetTimer(World);
}

void USL_GA_Player_ComboAttack::UnbindActiveMontageTask()
{
	if (!ActiveMontageTask) return;

	ActiveMontageTask->OnCompleted.RemoveDynamic(this, &USL_GA_Player_ComboAttack::OnComboMontageCompleted);
	ActiveMontageTask->OnBlendOut.RemoveDynamic(this, &USL_GA_Player_ComboAttack::OnComboMontageCompleted);
	ActiveMontageTask->OnInterrupted.RemoveDynamic(this, &USL_GA_Player_ComboAttack::OnComboMontageCompleted);
	ActiveMontageTask->OnCancelled.RemoveDynamic(this, &USL_GA_Player_ComboAttack::OnComboMontageCompleted);
	ActiveMontageTask = nullptr;
}

void USL_GA_Player_ComboAttack::StartComboResetTimer(UWorld* World)
{
	if (!World) return;

	World->GetTimerManager().SetTimer(
		ComboCountResetTimerHandle,
		this,
		&USL_GA_Player_ComboAttack::ResetComboCount,
		ComboResetTime,
		false
	);
}

void USL_GA_Player_ComboAttack::OnComboMontageCompleted()
{
	if (bMontageEndHandled || bComboChainRequested) return;

	bMontageEndHandled = true;

	if (bComboInputBuffered)
	{
		RequestComboChain();
		return;
	}

	UWorld* World = GetWorld();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	StartComboResetTimer(World);
}