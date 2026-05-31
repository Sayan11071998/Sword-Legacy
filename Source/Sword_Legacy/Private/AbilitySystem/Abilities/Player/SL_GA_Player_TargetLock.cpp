#include "AbilitySystem/Abilities/Player/SL_GA_Player_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/SL_WidgetBase.h"
#include "Controllers/SL_PlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"

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
		SetTargetLockWidgetPosition();
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

void USL_GA_Player_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}
	
	FVector2D ScreenPosition;
	
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetPlayerControllerFromActorInfo(),
		CurrentLockedActor->GetActorLocation(),
		ScreenPosition,
		true
	);
	
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
		[this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}	
		);
	}
	
	ScreenPosition -= TargetLockWidgetSize / 2.f;
	
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}

void USL_GA_Player_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void USL_GA_Player_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockedActor ||
		USL_FunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, SL_GameplayTags::Shared_Status_Death) || 
		USL_FunctionLibrary::NativeDoesActorHaveTag(GetPlayerCharacterFromActorInfo(), SL_GameplayTags::Shared_Status_Death))
	{
		CancelTargetLockAbility();
		return;
	}
	
	SetTargetLockWidgetPosition();
}

void USL_GA_Player_TargetLock::Cleanup()
{
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;
	
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
	
	DrawnTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;
}