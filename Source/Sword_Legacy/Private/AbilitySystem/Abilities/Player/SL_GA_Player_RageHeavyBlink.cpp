#include "AbilitySystem/Abilities/Player/SL_GA_Player_RageHeavyBlink.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttackMaster.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Items/Weapons/SL_PlayerWeapon.h"
#include "TimerManager.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Player_RageHeavyBlink::USL_GA_Player_RageHeavyBlink()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(SL_GameplayTags::Player_Ability_RageHeavyBlink);
	ActivationRequiredTags.AddTag(SL_GameplayTags::Player_Status_Rage_Active);
}

void USL_GA_Player_RageHeavyBlink::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	if (!AvatarActor || !USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_Rage_Active))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ResolveHeavyAttackDamage();
	BeginBlinkChain();
}

void USL_GA_Player_RageHeavyBlink::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BlinkStepTimer);
		World->GetTimerManager().ClearTimer(BlinkTravelTimer);
	}

	RestoreBlinkBody();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Player_RageHeavyBlink::ResolveHeavyAttackDamage()
{
	ResolvedDamageEffectClass = DamageEffectClass;
	ResolvedHitSoundCue = WeaponHitSoundGameplayCueTag;

	if (ResolvedDamageEffectClass && ResolvedHitSoundCue.IsValid()) return;

	const UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();

	if (!AbilitySystem) return;

	for (const FGameplayAbilitySpec& Spec : AbilitySystem->GetActivatableAbilities())
	{
		const USL_GA_Player_HeavyAttackMaster* HeavyAttack = Cast<USL_GA_Player_HeavyAttackMaster>(Spec.Ability);

		if (!HeavyAttack) continue;

		if (!ResolvedDamageEffectClass)
		{
			ResolvedDamageEffectClass = HeavyAttack->GetDamageEffectClass();
		}

		if (!ResolvedHitSoundCue.IsValid())
		{
			ResolvedHitSoundCue = HeavyAttack->GetWeaponHitSoundGameplayCueTag();
		}

		break;
	}
}

void USL_GA_Player_RageHeavyBlink::BeginBlinkChain()
{
	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();

	if (!Player || !FindBlinkTarget())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	BlinkPassIndex = 0;
	BlinkTarget = nullptr;

	if (UCapsuleComponent* Capsule = Player->GetCapsuleComponent())
	{
		SavedCapsuleCollision = Capsule->GetCollisionEnabled();
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (UCharacterMovementComponent* Movement = Player->GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	Player->SetAnimRootMotionTranslationScale(0.f);

	if (USkeletalMeshComponent* CharacterMesh = Player->GetMesh())
	{
		if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.05f);
		}
	}

	bBlinkBodyAltered = true;

	SetBlinkHidden(true);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			BlinkStepTimer,
			this,
			&USL_GA_Player_RageHeavyBlink::BeginBlinkPass,
			BlinkVanishTime,
			false
		);
	}
}

void USL_GA_Player_RageHeavyBlink::BeginBlinkPass()
{
	if (!IsActive()) return;

	if (BlinkPassIndex >= BlinkPassCount)
	{
		FinishBlinkChain();
		return;
	}

	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();
	AActor* Target = FindBlinkTarget();
	
	if (!Player || !Target)
	{
		FinishBlinkChain();
		return;
	}

	BlinkTarget = Target;

	const FVector PlayerLocation = Player->GetActorLocation();
	FVector ToTarget = Target->GetActorLocation() - PlayerLocation;
	ToTarget.Z = 0.f;

	FVector BaseDirection = ToTarget.GetSafeNormal();

	if (BaseDirection.IsNearlyZero())
	{
		BaseDirection = Player->GetActorForwardVector().GetSafeNormal2D();
	}

	if (BaseDirection.IsNearlyZero())
	{
		BaseDirection = FVector::ForwardVector;
	}

	const float AngleStep = BlinkPassCount > 0 ? 360.f / static_cast<float>(BlinkPassCount) : 90.f;
	const FVector PassDirection = BaseDirection.RotateAngleAxis(AngleStep * BlinkPassIndex, FVector::UpVector);
	const FVector EnemyLocation = Target->GetActorLocation();

	BlinkPassStart = EnemyLocation - PassDirection * BlinkPassDistance;
	BlinkPassEnd = EnemyLocation + PassDirection * BlinkPassDistance;
	BlinkPassStart.Z = PlayerLocation.Z;
	BlinkPassEnd.Z = PlayerLocation.Z;

	Player->SetActorLocation(BlinkPassStart, false);
	Player->SetActorRotation(FRotator(0.f, PassDirection.Rotation().Yaw, 0.f));
	
	SetBlinkHidden(false);

	if (BlinkPassMontage)
	{
		if (USkeletalMeshComponent* CharacterMesh = Player->GetMesh())
		{
			if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
			{
				AnimInstance->Montage_Play(BlinkPassMontage, 1.5f);
			}
		}
	}

	bBlinkDamageApplied = false;
	BlinkTravelAlpha = 0.f;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			BlinkTravelTimer,
			this,
			&USL_GA_Player_RageHeavyBlink::TravelBlinkPass,
			0.016f,
			true
		);
	}
}

void USL_GA_Player_RageHeavyBlink::TravelBlinkPass()
{
	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();

	if (!Player)
	{
		FinishBlinkChain();
		return;
	}

	const float Step = BlinkTravelTime > 0.f ? 0.016f / BlinkTravelTime : 1.f;
	BlinkTravelAlpha = FMath::Min(1.f, BlinkTravelAlpha + Step);

	const FVector Location = FMath::Lerp(BlinkPassStart, BlinkPassEnd, BlinkTravelAlpha);
	Player->SetActorLocation(Location, false);

	if (!bBlinkDamageApplied && BlinkTravelAlpha >= 0.45f)
	{
		bBlinkDamageApplied = true;
		ApplyBlinkDamage(BlinkTarget.Get());
	}

	if (BlinkTravelAlpha < 1.f) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BlinkTravelTimer);
	}

	SetBlinkHidden(true);

	++BlinkPassIndex;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			BlinkStepTimer,
			this,
			&USL_GA_Player_RageHeavyBlink::BeginBlinkPass,
			BlinkVanishTime,
			false
		);
	}
}

void USL_GA_Player_RageHeavyBlink::FinishBlinkChain()
{
	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();

	if (Player && !BlinkPassEnd.IsNearlyZero())
	{
		Player->SetActorLocation(BlinkPassEnd, false);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Player_RageHeavyBlink::SetBlinkHidden(bool bHidden)
{
	if (ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo())
	{
		Player->SetActorHiddenInGame(bHidden);
	}

	if (USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo())
	{
		if (ASL_PlayerWeapon* Weapon = CombatComponent->GetPlayerCurrentEquippedWeapon())
		{
			Weapon->SetActorHiddenInGame(bHidden);
		}
	}
}

void USL_GA_Player_RageHeavyBlink::RestoreBlinkBody()
{
	if (!bBlinkBodyAltered) return;

	bBlinkBodyAltered = false;

	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();

	if (!Player) return;

	SetBlinkHidden(false);

	Player->SetAnimRootMotionTranslationScale(1.f);

	if (UCapsuleComponent* Capsule = Player->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(SavedCapsuleCollision);
	}

	if (UCharacterMovementComponent* Movement = Player->GetCharacterMovement())
	{
		Movement->SetMovementMode(MOVE_Walking);
	}

	if (USkeletalMeshComponent* CharacterMesh = Player->GetMesh())
	{
		if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.05f);
		}
	}
}

void USL_GA_Player_RageHeavyBlink::ApplyBlinkDamage(AActor* Target)
{
	if (!IsLivingEnemy(Target) || !ResolvedDamageEffectClass) return;

	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	
	if (!CombatComponent) return;

	const float WeaponDamage = CombatComponent->GetPlayerCurrentEquippedWeaponDamageAtLevel(
		static_cast<float>(GetAbilityLevel()) * RageDamageMultiplier
	);

	const FGameplayEffectSpecHandle DamageSpecHandle = MakePlayerDamageEffectSpecHandle(
		ResolvedDamageEffectClass,
		WeaponDamage,
		SL_GameplayTags::Player_SetByCaller_AttackType_Heavy,
		RageComboCount
	);

	if (!DamageSpecHandle.IsValid()) return;

	if (ResolvedHitSoundCue.IsValid())
	{
		FGameplayEffectContextHandle ContextHandle = DamageSpecHandle.Data->GetContext();
		K2_ExecuteGameplayCue(ResolvedHitSoundCue, ContextHandle);
	}

	const FActiveGameplayEffectHandle ActiveEffectHandle = NativeApplyEffectSpecHandleToTarget(Target, DamageSpecHandle);
	
	if (!ActiveEffectHandle.WasSuccessfullyApplied()) return;

	FGameplayEventData HitReactPayload;
	HitReactPayload.Instigator = GetAvatarActorFromActorInfo();
	HitReactPayload.Target = Target;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Target,
		SL_GameplayTags::Shared_Event_HitReact,
		HitReactPayload
	);
}

TObjectPtr<AActor> USL_GA_Player_RageHeavyBlink::FindBlinkTarget()
{
	ASL_PlayerCharacter* Player = GetPlayerCharacterFromActorInfo();

	if (!Player) return nullptr;

	if (AActor* CurrentTarget = BlinkTarget.Get())
	{
		if (IsLivingEnemy(CurrentTarget) &&
			FVector::Dist2D(Player->GetActorLocation(), CurrentTarget->GetActorLocation()) <= BlinkSearchDistance)
		{
			return CurrentTarget;
		}
	}

	UWorld* World = Player->GetWorld();
	
	if (!World) return nullptr;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RageBlinkTarget), false, Player);
	FCollisionObjectQueryParams ObjectParams;
	
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	World->OverlapMultiByObjectType(
		Overlaps,
		Player->GetActorLocation(),
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(BlinkSearchDistance),
		QueryParams
	);

	const bool bPreviousTargetDied = BlinkTarget.IsValid() && !IsLivingEnemy(BlinkTarget.Get());
	const FVector SearchOrigin = bPreviousTargetDied ? BlinkTarget->GetActorLocation() : Player->GetActorLocation();

	const AController* Controller = Player->GetController();

	FVector AimDirection = Controller
		? Controller->GetControlRotation().Vector().GetSafeNormal2D()
		: Player->GetActorForwardVector().GetSafeNormal2D();
	
	if (AimDirection.IsNearlyZero())
	{
		AimDirection = Player->GetActorForwardVector().GetSafeNormal2D();
	}

	AActor* BestEnemy = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Enemy = Overlap.GetActor();

		if (!IsLivingEnemy(Enemy)) continue;

		float Score = FVector::DistSquared2D(SearchOrigin, Enemy->GetActorLocation());

		if (!bPreviousTargetDied && !AimDirection.IsNearlyZero())
		{
			const FVector Direction = (Enemy->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal2D();

			if (Direction.IsNearlyZero()) continue;

			const float Dot = FVector::DotProduct(AimDirection, Direction);
			const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f)));
			
			if (AngleDegrees > 70.f) continue;

			const float Distance = FVector::Dist2D(Player->GetActorLocation(), Enemy->GetActorLocation());
			const float DistanceBias = BlinkSearchDistance > 0.f ? (Distance / BlinkSearchDistance) * 12.f : 0.f;
			Score = AngleDegrees + DistanceBias;
		}

		if (Score < BestScore)
		{
			BestScore = Score;
			BestEnemy = Enemy;
		}
	}

	if (BestEnemy || bPreviousTargetDied) return BestEnemy;

	BestScore = TNumericLimits<float>::Max();
	
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Enemy = Overlap.GetActor();
		
		if (!IsLivingEnemy(Enemy)) continue;

		const float DistanceSq = FVector::DistSquared2D(Player->GetActorLocation(), Enemy->GetActorLocation());
		if (DistanceSq < BestScore)
		{
			BestScore = DistanceSq;
			BestEnemy = Enemy;
		}
	}

	return BestEnemy;
}

bool USL_GA_Player_RageHeavyBlink::IsLivingEnemy(AActor* Enemy) const
{
	if (!Cast<ASL_EnemyCharacter>(Enemy)) return false;

	return !USL_FunctionLibrary::NativeDoesActorHaveTag(Enemy, SL_GameplayTags::Shared_Status_Death);
}