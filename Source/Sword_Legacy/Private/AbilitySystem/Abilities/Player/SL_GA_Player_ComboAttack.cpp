#include "AbilitySystem/Abilities/Player/SL_GA_Player_ComboAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"

void USL_GA_Player_ComboAttack::PrepareComboAttack()
{
	bComboWindowOpen = false;
	bComboInputBuffered = false;
	bComboChainRequested = false;
	bMontageEndHandled = false;
	PlayingMontage = nullptr;

	UnbindActiveMontageTask();

	if (ComboCountResetTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboCountResetTimerHandle);
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

void USL_GA_Player_ComboAttack::ResetComboCount() { }

void USL_GA_Player_ComboAttack::ReceiveComboInput()
{
	if (!IsActive() || bComboChainRequested) return;

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

	const FGameplayAbilitySpecHandle Handle = CurrentSpecHandle;

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