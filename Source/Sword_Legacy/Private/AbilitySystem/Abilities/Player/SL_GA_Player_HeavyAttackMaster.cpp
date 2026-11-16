#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttackMaster.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_TargetLock.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "MotionWarpingComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

USL_GA_Player_HeavyAttackMaster::USL_GA_Player_HeavyAttackMaster()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Player_HeavyAttackMaster::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PrepareComboAttack();
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const bool bJumpToFinisher = AvatarActor &&
		USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);

	int32 ComboCount = GetComboCount();

	if (bJumpToFinisher)
	{
		ComboCount = HeavyAttackMontagesMap.Num();
	}

	UsedComboCount = ComboCount;
	
	TObjectPtr<UAnimMontage>* MontageToPlay = HeavyAttackMontagesMap.Find(ComboCount);

	if (!MontageToPlay || !(*MontageToPlay))
	{
		ComboCount = 1;
		MontageToPlay = HeavyAttackMontagesMap.Find(ComboCount);

		if (!MontageToPlay || !(*MontageToPlay))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}

	UAbilityTask_WaitGameplayEvent* WaitMeleeHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Shared_Event_MeleeHit,
		nullptr,
		false,
		true
	);
	WaitMeleeHitTask->EventReceived.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMeleeHitEventReceived);
	WaitMeleeHitTask->ReadyForActivation();
	
	SetupHeavyAttackMotionWarp();

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, *MontageToPlay, 1.0f
	);
	BindComboMontage(PlayMontageTask, *MontageToPlay);
	PlayMontageTask->ReadyForActivation();
	
	const int32 TotalMontages = HeavyAttackMontagesMap.Num();

	if (ComboCount == TotalMontages)
	{
		ResetComboCount();
	}
	else
	{
		SetComboCount(ComboCount + 1);
	}

	if (AvatarActor && USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_Rage_Active))
	{
		WhileRageActive();
	}
}

void USL_GA_Player_HeavyAttackMaster::OnMeleeHitEventReceived(FGameplayEventData Payload)
{
	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;

	const float WeaponBaseDamage = CombatComponent->GetPlayerCurrentEquippedWeaponDamageAtLevel(static_cast<float>(GetAbilityLevel()));

	const FGameplayEffectSpecHandle DamageSpecHandle = MakePlayerDamageEffectSpecHandle(
		DamageEffectClass,
		WeaponBaseDamage,
		SL_GameplayTags::Player_SetByCaller_AttackType_Heavy,
		UsedComboCount
	);

	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());
	if (TargetActor && DamageSpecHandle.IsValid())
	{
		if (WeaponHitSoundGameplayCueTag.IsValid())
		{
			FGameplayEffectContextHandle ContextHandle = DamageSpecHandle.Data->GetContext();
			K2_ExecuteGameplayCue(WeaponHitSoundGameplayCueTag, ContextHandle);
		}
		
		const FActiveGameplayEffectHandle ActiveEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, DamageSpecHandle);

		if (ActiveEffectHandle.WasSuccessfullyApplied())
		{
			FGameplayEventData HitReactPayload;
			HitReactPayload.Instigator = GetAvatarActorFromActorInfo();
			HitReactPayload.Target = TargetActor;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				TargetActor,
				SL_GameplayTags::Shared_Event_HitReact,
				HitReactPayload
			);

			if (GainRageEffectClass)
			{
				if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
				{
					FGameplayEffectSpecHandle RageSpecHandle = MakeOutgoingGameplayEffectSpec(GainRageEffectClass, GetAbilityLevel());
					if (RageSpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*RageSpecHandle.Data.Get());
					}
				}
			}
		}
	}
}

void USL_GA_Player_HeavyAttackMaster::SetupHeavyAttackMotionWarp()
{
	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();
	if (!Player || HeavyAttackWarpTargetName.IsNone()) return;

	UMotionWarpingComponent* MotionWarping = Player->GetMotionWarpingComponent();
	if (!MotionWarping) return;

	AActor* Target = FindHeavyAttackWarpTarget(Player);
	if (!Target)
	{
		MotionWarping->RemoveWarpTarget(HeavyAttackWarpTargetName);
		return;
	}

	const FVector PlayerLocation = Player->GetActorLocation();
	FVector ToTarget = Target->GetActorLocation() - PlayerLocation;
	ToTarget.Z = 0.f;

	const FVector Direction = ToTarget.GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		MotionWarping->RemoveWarpTarget(HeavyAttackWarpTargetName);
		return;
	}

	FVector WarpLocation = Target->GetActorLocation() - Direction * HeavyAttackStopDistance;
	WarpLocation.Z = PlayerLocation.Z;

	const FRotator WarpRotation(0.f, Direction.Rotation().Yaw, 0.f);
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(HeavyAttackWarpTargetName, WarpLocation, WarpRotation);
}

TObjectPtr<AActor> USL_GA_Player_HeavyAttackMaster::FindHeavyAttackWarpTarget(const TObjectPtr<ASL_PlayerCharacter> Player) const
{
	if (!Player) return nullptr;

	if (AActor* LockedTarget = FindLockedTarget())
	{
		if (IsEnemyInWarpRange(Player, LockedTarget))
		{
			return LockedTarget;
		}
	}

	UWorld* World = Player->GetWorld();
	if (!World) return nullptr;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(HeavyAttackWarp), false, Player);
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	World->OverlapMultiByObjectType(
		Overlaps,
		Player->GetActorLocation(),
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(HeavyAttackWarpDistance),
		QueryParams
	);

	const FVector Forward = Player->GetActorForwardVector().GetSafeNormal2D();
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(HeavyAttackWarpHalfAngle));

	AActor* BestEnemy = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Enemy = Overlap.GetActor();
		if (!IsEnemyInWarpRange(Player, Enemy)) continue;

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

TObjectPtr<AActor> USL_GA_Player_HeavyAttackMaster::FindLockedTarget() const
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

bool USL_GA_Player_HeavyAttackMaster::IsEnemyInWarpRange(const ASL_PlayerCharacter* Player, AActor* Enemy) const
{
	ASL_EnemyCharacter* EnemyCharacter = Cast<ASL_EnemyCharacter>(Enemy);
	
	if (!Player || !EnemyCharacter) return false;
	
	if (USL_FunctionLibrary::NativeDoesActorHaveTag(EnemyCharacter, SL_GameplayTags::Shared_Status_Death)) return false;

	return FVector::Dist2D(Player->GetActorLocation(), EnemyCharacter->GetActorLocation()) <= HeavyAttackWarpDistance;
}

void USL_GA_Player_HeavyAttackMaster::WhileRageActive_Implementation() { }