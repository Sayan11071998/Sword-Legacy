#include "AbilitySystem/Abilities/Player/SL_GA_Player_Roll.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/SL_PlayerCharacter.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

USL_GA_Player_Roll::USL_GA_Player_Roll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	DelayDuration = 0.05f;
	TraceHeight = 500.f;
	RotationWarpTargetName = FName("RollingDirection");
	LocationWarpTargetName = FName("RollTargetLocation");
}

void USL_GA_Player_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, DelayDuration);
	if (DelayTask)
	{
		DelayTask->OnFinish.AddDynamic(this, &USL_GA_Player_Roll::OnDelayFinished);
		DelayTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void USL_GA_Player_Roll::ComputeRollDirectionAndDistance()
{
	ASL_PlayerCharacter* PlayerChar = GetPlayerCharacterFromActorInfo();
	if (!PlayerChar) return;
	
	UMotionWarpingComponent* MotionWarpComp = PlayerChar->GetMotionWarpingComponent();
	if (!MotionWarpComp) return;

	FVector LastInput = PlayerChar->GetLastMovementInputVector();
	FVector RollDirection = LastInput.IsNearlyZero(0.0001f) ? PlayerChar->GetActorForwardVector() : LastInput.GetSafeNormal(0.0001f);
	FRotator RollRotation = RollDirection.Rotation();
	
	MotionWarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(
		RotationWarpTargetName,
		FVector::ZeroVector,
		RollRotation
	);

	float RollDistance = RollingDistanceScalableFloat.GetValueAtLevel(GetAbilityLevel());

	FVector ActorLocation = PlayerChar->GetActorLocation();
	FVector Start = ActorLocation + (RollDirection * RollDistance);
	FVector End = Start + (PlayerChar->GetActorUpVector() * -TraceHeight);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(PlayerChar);
	
	FHitResult HitResult;
	
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	FVector TargetLocation = Start;
	if (bHit)
	{
		TargetLocation = HitResult.ImpactPoint;
	}

	MotionWarpComp->AddOrUpdateWarpTargetFromLocation(
		LocationWarpTargetName,
		TargetLocation
	);
}

void USL_GA_Player_Roll::OnDelayFinished()
{
	ComputeRollDirectionAndDistance();
	
	if (!RollMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		RollMontage,
		1.f
	);
	
	if (PlayMontageTask)
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_Roll::OnMontageCompleted);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_Roll::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_Roll::OnMontageCompleted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_Roll::OnMontageCompleted);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void USL_GA_Player_Roll::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}