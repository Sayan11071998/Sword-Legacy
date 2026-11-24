#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_TargetLock.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Controllers/SL_PlayerController.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "MotionWarpingComponent.h"
#include "MotionWarpingAdapter.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Utilities/SL_FunctionLibrary.h"

ASL_PlayerCharacter* USL_PlayerGameplayAbility::GetPlayerCharacterFromActorInfo()
{
	if (!CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter = Cast<ASL_PlayerCharacter>(CurrentActorInfo->AvatarActor);
	}
	
	return CachedPlayerCharacter.IsValid() ? CachedPlayerCharacter.Get() : nullptr;
}

ASL_PlayerController* USL_PlayerGameplayAbility::GetPlayerControllerFromActorInfo()
{
	if (!CachedPlayerController.IsValid())
	{
		CachedPlayerController = Cast<ASL_PlayerController>(CurrentActorInfo->PlayerController);
	}
	
	return CachedPlayerController.IsValid() ? CachedPlayerController.Get() : nullptr;
}

USL_PlayerCombatComponent* USL_PlayerGameplayAbility::GetPlayerCombatComponentFromActorInfo()
{
	if (ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		return PlayerCharacter->GetPlayerCombatComponent();
	}
	
	return nullptr;
}

USL_PlayerUIComponent* USL_PlayerGameplayAbility::GetPlayerUIComponentFromActorInfo()
{
	if (ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		return PlayerCharacter->GetPlayerUIComponent();
	}
	
	return nullptr;
}

FGameplayEffectSpecHandle USL_PlayerGameplayAbility::MakePlayerDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);
	
	FGameplayEffectContextHandle ContextHandle = GetPawnAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	
	FGameplayEffectSpecHandle EffectSpecHandle = GetPawnAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);
	
	EffectSpecHandle.Data->SetSetByCallerMagnitude(SL_GameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);
	
	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
	}
	
	return EffectSpecHandle;
}

bool USL_PlayerGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime,
	float& RemainingCooldownTime)
{
	check(InCooldownTag.IsValid());
	
	FGameplayEffectQuery CooldownQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());
	
	TArray<TPair<float, float>> TimeRemainingAndDuration = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);
	
	if (!TimeRemainingAndDuration.IsEmpty())
	{
		RemainingCooldownTime = TimeRemainingAndDuration[0].Key;
		TotalCooldownTime = TimeRemainingAndDuration[0].Value;
	}
	
	return RemainingCooldownTime > 0.f;
}

void USL_PlayerGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearMeleeMotionWarp();
	ClearIncomingAttackWarnings();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_PlayerGameplayAbility::WarnEnemyOfIncomingAttack(AActor* Enemy)
{
	if (!Enemy) return;

	if (USL_FunctionLibrary::NativeDoesActorHaveTag(Enemy, SL_GameplayTags::Shared_Status_Death)) return;

	USL_FunctionLibrary::AddGameplayTagToActorIfNone(Enemy, SL_GameplayTags::Enemy_Status_IncomingAttack);

	for (const TWeakObjectPtr<AActor>& WarnedEnemy : WarnedEnemies)
	{
		if (WarnedEnemy.Get() == Enemy) return;
	}

	WarnedEnemies.Add(Enemy);
}

void USL_PlayerGameplayAbility::ClearIncomingAttackWarnings()
{
	for (const TWeakObjectPtr<AActor>& WarnedEnemy : WarnedEnemies)
	{
		AActor* Enemy = WarnedEnemy.Get();

		if (!Enemy) continue;

		if (USL_FunctionLibrary::NativeDoesActorHaveTag(Enemy, SL_GameplayTags::Enemy_Status_Blocking)) continue;

		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(Enemy, SL_GameplayTags::Enemy_Status_IncomingAttack);
	}

	WarnedEnemies.Reset();
}

void USL_PlayerGameplayAbility::SetupMeleeMotionWarp(FName WarpTargetName, float WarpDistance, float StopDistance, float HalfAngle)
{
	if (WarpTargetName.IsNone()) return;

	ActiveWarpTargetName = WarpTargetName;
	ActiveWarpDistance = WarpDistance;
	ActiveWarpStopDistance = StopDistance;
	ActiveWarpHalfAngle = HalfAngle;

	ApplyMeleeMotionWarp();
	MaintainMeleeSpacing();

	if (ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo())
	{
		Player->OnCharacterMovementUpdated.RemoveDynamic(this, &USL_PlayerGameplayAbility::OnMeleeMovementUpdated);
		Player->OnCharacterMovementUpdated.AddDynamic(this, &USL_PlayerGameplayAbility::OnMeleeMovementUpdated);

		if (UMotionWarpingComponent* MotionWarping = Player->GetMotionWarpingComponent())
		{
			MotionWarping->OnPreUpdate.RemoveDynamic(this, &USL_PlayerGameplayAbility::OnMeleeWarpPreUpdate);
			MotionWarping->OnPreUpdate.AddDynamic(this, &USL_PlayerGameplayAbility::OnMeleeWarpPreUpdate);
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			MeleeWarpTimer,
			FTimerDelegate::CreateUObject(this, &USL_PlayerGameplayAbility::ApplyMeleeMotionWarp),
			0.016f,
			true
		);
	}
}

void USL_PlayerGameplayAbility::ClearMeleeMotionWarp()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MeleeWarpTimer);
	}

	if (ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo())
	{
		Player->OnCharacterMovementUpdated.RemoveDynamic(this, &USL_PlayerGameplayAbility::OnMeleeMovementUpdated);
		Player->SetAnimRootMotionTranslationScale(1.f);

		if (UMotionWarpingComponent* MotionWarping = Player->GetMotionWarpingComponent())
		{
			MotionWarping->OnPreUpdate.RemoveDynamic(this, &USL_PlayerGameplayAbility::OnMeleeWarpPreUpdate);
		}

		if (!ActiveWarpTargetName.IsNone())
		{
			if (UMotionWarpingComponent* MotionWarping = Player->GetMotionWarpingComponent())
			{
				MotionWarping->RemoveWarpTarget(ActiveWarpTargetName);
			}
		}
	}

	if (SpacingModifier.IsValid())
	{
		SpacingModifier->SourceAbility = nullptr;
		SpacingModifier->SetState(ERootMotionModifierState::MarkedForRemoval);
		SpacingModifier = nullptr;
	}

	ActiveWarpTargetName = NAME_None;
}

void USL_PlayerGameplayAbility::OnMeleeMovementUpdated(float /*DeltaSeconds*/, FVector /*OldLocation*/, FVector /*OldVelocity*/)
{
	MaintainMeleeSpacing();
}

void USL_PlayerGameplayAbility::OnMeleeWarpPreUpdate(UMotionWarpingComponent* MotionWarping)
{
	if (!MotionWarping || ActiveWarpTargetName.IsNone() || ActiveWarpStopDistance <= 0.f) return;

	const ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();

	if (!Player || !Player->GetRootMotionAnimMontageInstance()) return;

	if (SpacingModifier.IsValid())
	{
		SpacingModifier->SourceAbility = nullptr;
		SpacingModifier->SetState(ERootMotionModifierState::MarkedForRemoval);
	}

	USL_RootMotionModifier_MeleeSpacing* NewModifier = NewObject<USL_RootMotionModifier_MeleeSpacing>(MotionWarping);
	NewModifier->SourceAbility = this;
	NewModifier->StartTime = 0.f;
	NewModifier->EndTime = 1000.f;

	if (const FAnimMontageInstance* MontageInstance = Player->GetRootMotionAnimMontageInstance())
	{
		NewModifier->Animation = MontageInstance->Montage;
	}

	MotionWarping->AddModifier(NewModifier);
	SpacingModifier = NewModifier;
}

AActor* USL_PlayerGameplayAbility::GetMeleeSpacingTarget()
{
	const ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();

	if (!Player || ActiveWarpTargetName.IsNone()) return nullptr;

	return FindMeleeWarpTarget(Player, ActiveWarpDistance, ActiveWarpHalfAngle);
}

void USL_RootMotionModifier_MeleeSpacing::Update(const FMotionWarpingUpdateContext& Context)
{
	if (!SourceAbility.IsValid())
	{
		SetState(ERootMotionModifierState::MarkedForRemoval);
		return;
	}

	PreviousPosition = Context.PreviousPosition;
	CurrentPosition = Context.CurrentPosition;
	Weight = Context.Weight;
	PlayRate = Context.PlayRate;

	if (GetState() != ERootMotionModifierState::Active)
	{
		SetState(ERootMotionModifierState::Active);
	}
}

FTransform USL_RootMotionModifier_MeleeSpacing::ProcessRootMotion(const FTransform& InRootMotion, float /*DeltaSeconds*/)
{
	USL_PlayerGameplayAbility* Ability = SourceAbility.Get();

	const UMotionWarpingBaseAdapter* Adapter = GetOwnerAdapter();

	AActor* Actor = GetActorOwner();

	if (!Ability || !Adapter || !Actor) return InRootMotion;

	AActor* Target = Ability->GetMeleeSpacingTarget();
	if (!Target) return InRootMotion;

	const float StopDistance = Ability->GetActiveMeleeStopDistance();
	if (StopDistance <= 0.f) return InRootMotion;

	const FTransform MeshTransform = FTransform(Adapter->GetBaseVisualRotationOffset(), Adapter->GetBaseVisualTranslationOffset()) * Actor->GetActorTransform();
	FVector ToTarget = MeshTransform.InverseTransformPositionNoScale(Target->GetActorLocation());
	ToTarget.Z = 0.f;

	const float Distance = ToTarget.Size();

	FVector Direction = ToTarget.GetSafeNormal();

	if (Direction.IsNearlyZero())
	{
		FVector LocalForward = MeshTransform.InverseTransformVectorNoScale(Actor->GetActorForwardVector());
		LocalForward.Z = 0.f;
		Direction = LocalForward.GetSafeNormal();
	}

	if (Direction.IsNearlyZero()) return InRootMotion;

	FVector Delta = InRootMotion.GetTranslation();
	FVector Horizontal = Delta;
	Horizontal.Z = 0.f;

	const float Inward = FVector::DotProduct(Horizontal, Direction);
	const float AllowedInward = FMath::Max(0.f, Distance - StopDistance);

	if (Inward <= AllowedInward) return InRootMotion;

	Horizontal -= Direction * (Inward - AllowedInward);
	Delta.X = Horizontal.X;
	Delta.Y = Horizontal.Y;

	FTransform ClampedRootMotion = InRootMotion;
	ClampedRootMotion.SetTranslation(Delta);

	return ClampedRootMotion;
}

void USL_PlayerGameplayAbility::MaintainMeleeSpacing()
{
	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();

	if (!Player || ActiveWarpTargetName.IsNone() || ActiveWarpStopDistance <= 0.f) return;

	AActor* Target = FindMeleeWarpTarget(Player, ActiveWarpDistance, ActiveWarpHalfAngle);

	if (!Target)
	{
		Player->SetAnimRootMotionTranslationScale(1.f);
		return;
	}

	const FVector PlayerLocation = Player->GetActorLocation();
	FVector ToTarget = Target->GetActorLocation() - PlayerLocation;
	ToTarget.Z = 0.f;

	const float Distance = ToTarget.Size();
	if (Distance > ActiveWarpStopDistance + 2.f)
	{
		Player->SetAnimRootMotionTranslationScale(1.f);
		return;
	}

	if (Distance < ActiveWarpStopDistance)
	{
		FVector Direction = ToTarget.GetSafeNormal();
		if (Direction.IsNearlyZero())
		{
			Direction = Player->GetActorForwardVector().GetSafeNormal2D();
		}

		if (!Direction.IsNearlyZero())
		{
			FVector HoldLocation = PlayerLocation - Direction * (ActiveWarpStopDistance - Distance);
			HoldLocation.Z = PlayerLocation.Z;
			Player->SetActorLocation(HoldLocation, false);
		}
	}

	Player->SetAnimRootMotionTranslationScale(0.f);
}

void USL_PlayerGameplayAbility::ApplyMeleeMotionWarp()
{
	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();
	if (!Player || ActiveWarpTargetName.IsNone()) return;

	UMotionWarpingComponent* MotionWarping = Player->GetMotionWarpingComponent();
	if (!MotionWarping) return;

	AActor* Target = FindMeleeWarpTarget(Player, ActiveWarpDistance, ActiveWarpHalfAngle);
	if (!Target)
	{
		MotionWarping->RemoveWarpTarget(ActiveWarpTargetName);
		return;
	}

	const FVector PlayerLocation = Player->GetActorLocation();

	FVector ToTarget = Target->GetActorLocation() - PlayerLocation;
	ToTarget.Z = 0.f;

	const FVector Direction = ToTarget.GetSafeNormal();

	if (Direction.IsNearlyZero())
	{
		MotionWarping->RemoveWarpTarget(ActiveWarpTargetName);
		return;
	}

	FVector WarpLocation = Target->GetActorLocation() - Direction * ActiveWarpStopDistance;
	WarpLocation.Z = PlayerLocation.Z;

	const FRotator WarpRotation(0.f, Direction.Rotation().Yaw, 0.f);

	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(ActiveWarpTargetName, WarpLocation, WarpRotation);
}

TObjectPtr<AActor> USL_PlayerGameplayAbility::FindMeleeWarpTarget(const ASL_PlayerCharacter* Player, float WarpDistance, float HalfAngle) const
{
	if (!Player) return nullptr;

	if (AActor* LockedTarget = FindLockedWarpTarget())
	{
		if (IsEnemyInMeleeWarpRange(Player, LockedTarget, WarpDistance))
		{
			return LockedTarget;
		}
	}

	UWorld* World = Player->GetWorld();
	if (!World) return nullptr;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MeleeAttackWarp), false, Player);
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	World->OverlapMultiByObjectType(
		Overlaps,
		Player->GetActorLocation(),
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(WarpDistance),
		QueryParams
	);

	const FVector Forward = Player->GetActorForwardVector().GetSafeNormal2D();
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(HalfAngle));

	AActor* BestEnemy = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Enemy = Overlap.GetActor();
		if (!IsEnemyInMeleeWarpRange(Player, Enemy, WarpDistance)) continue;

		const FVector ToEnemy = (Enemy->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal2D();
		if (FVector::DotProduct(Forward, ToEnemy) < MinDot) continue;

		const float DistanceSq = FVector::DistSquared2D(Player->GetActorLocation(), Enemy->GetActorLocation());
		
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestEnemy = Enemy;
		}
	}

	return BestEnemy;
}

TObjectPtr<AActor> USL_PlayerGameplayAbility::FindLockedWarpTarget() const
{
	const UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystemComponent) return nullptr;

	for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (!AbilitySpec.IsActive()) continue;

		for (const UGameplayAbility* Instance : AbilitySpec.GetAbilityInstances())
		{
			const USL_GA_Player_TargetLock* TargetLock = Cast<USL_GA_Player_TargetLock>(Instance);
			
			if (!TargetLock || !TargetLock->IsActive()) continue;

			return TargetLock->GetCurrentLockedActor();
		}
	}

	return nullptr;
}

bool USL_PlayerGameplayAbility::IsEnemyInMeleeWarpRange(const ASL_PlayerCharacter* Player, AActor* Enemy, float WarpDistance) const
{
	ASL_EnemyCharacter* EnemyCharacter = Cast<ASL_EnemyCharacter>(Enemy);

	if (!Player || !EnemyCharacter) return false;

	if (USL_FunctionLibrary::NativeDoesActorHaveTag(EnemyCharacter, SL_GameplayTags::Shared_Status_Death)) return false;

	return FVector::Dist2D(Player->GetActorLocation(), EnemyCharacter->GetActorLocation()) <= WarpDistance;
}