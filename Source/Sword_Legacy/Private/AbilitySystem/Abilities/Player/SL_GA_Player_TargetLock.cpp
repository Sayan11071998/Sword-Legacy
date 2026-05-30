#include "AbilitySystem/Abilities/Player/SL_GA_Player_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/SL_WidgetBase.h"
#include "Controllers/SL_PlayerController.h"

void USL_GA_Player_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USL_GA_Player_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Cleanup();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Player_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();
	
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	
	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);
	
	if (CurrentLockedActor)
	{
		DrawTargetLockWidget();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void USL_GA_Player_TargetLock::GetAvailableActorsToLock()
{
	TArray<FHitResult> BoxTraceHits;
	
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetPlayerCharacterFromActorInfo(),
		GetPlayerCharacterFromActorInfo()->GetActorLocation(),
		GetPlayerCharacterFromActorInfo()->GetActorLocation() + GetPlayerCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistance,
		TraceBoxSize / 2.f,
		GetPlayerCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);
	
	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != GetPlayerCharacterFromActorInfo())
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

TObjectPtr<AActor> USL_GA_Player_TargetLock::GetNearestTargetFromAvailableActors(
	const TArray<TObjectPtr<AActor>>& InAvailableActors)
{
	float ClosestDistance = 0.f;
	
	return UGameplayStatics::FindNearestActor(
		GetPlayerCharacterFromActorInfo()->GetActorLocation(),
		InAvailableActors,
		ClosestDistance
	);
}

void USL_GA_Player_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in blueprint"));
	
		DrawnTargetLockWidget = CreateWidget<USL_WidgetBase>(GetPlayerControllerFromActorInfo(), TargetLockWidgetClass);
	
		check(DrawnTargetLockWidget);
	
		DrawnTargetLockWidget->AddToViewport();
	}
}

void USL_GA_Player_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void USL_GA_Player_TargetLock::Cleanup()
{
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;
	
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
}