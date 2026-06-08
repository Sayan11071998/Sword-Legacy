#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_Projectile.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Items/Projectiles/SL_ProjectileBase.h"
#include "Characters/SL_EnemyCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

USL_GA_Enemy_Projectile::USL_GA_Enemy_Projectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ProjectileSpawnSocketName = FName(TEXT("ProjectileSpawnSocket"));
}

void USL_GA_Enemy_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SpawnProjectileEventTag,
		nullptr,
		false,
		true
	);
	WaitEventTask->EventReceived.AddDynamic(this, &USL_GA_Enemy_Projectile::OnProjectileSpawnEventReceived);
	WaitEventTask->ReadyForActivation();

	if (MontageToPlay)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			MontageToPlay
		);

		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_Projectile::OnMontageFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_Projectile::OnMontageFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_Projectile::OnMontageFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_Projectile::OnMontageFinished);
		
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USL_GA_Enemy_Projectile::OnProjectileSpawnEventReceived(FGameplayEventData Payload)
{
	ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo();
	if (!EnemyCharacter || !ProjectileClass) return;

	USkeletalMeshComponent* SkeletalMesh = EnemyCharacter->GetMesh();
	if (!SkeletalMesh) return;

	const FVector SpawnLocation = SkeletalMesh->GetSocketLocation(ProjectileSpawnSocketName);

	AActor* TargetActor = nullptr;
	if (AAIController* AIController = Cast<AAIController>(EnemyCharacter->GetController()))
	{
		if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent())
		{
			TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(FName(TEXT("TargetActor"))));
		}
	}

	FRotator SpawnRotation = EnemyCharacter->GetActorForwardVector().Rotation();
	if (TargetActor)
	{
		SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetActor->GetActorLocation());
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = EnemyCharacter;
	SpawnParams.Instigator = EnemyCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ASL_ProjectileBase* SpawnedProjectile = GetWorld()->SpawnActor<ASL_ProjectileBase>(
		ProjectileClass,
		FTransform(SpawnRotation, SpawnLocation),
		SpawnParams
	);

	if (SpawnedProjectile && ProjectileDamageEffectClass)
	{
		const FGameplayEffectSpecHandle DamageSpecHandle = MakeEnemyDamageEffectSpecHandle(
			ProjectileDamageEffectClass,
			ProjectileDamageScalableFloat
		);
		SpawnedProjectile->ProjectileDamageEffectSpecHandle = DamageSpecHandle;
	}
}

void USL_GA_Enemy_Projectile::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}