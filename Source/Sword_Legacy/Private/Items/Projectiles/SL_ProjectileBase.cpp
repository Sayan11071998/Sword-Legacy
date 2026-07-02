#include "Items/Projectiles/SL_ProjectileBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Components/Combat/SL_PawnCombatComponent.h"
#include "Items/Weapons/SL_WeaponBase.h"

ASL_ProjectileBase::ASL_ProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ASL_ProjectileBase::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASL_ProjectileBase::OnProjectileBeginOverlap);
	
	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());
	
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;
	
	InitialLifeSpan = 4.f;

	ProjectileImpactSound = nullptr;
	ProjectileImpactEffects = nullptr;
	ProjectileSpawnSound = nullptr;
	ProjectileFlyingSound = nullptr;
	ProjectileMuzzleFX = nullptr;
	ProjectileFlyingAudioComponent = nullptr;
}

void ASL_ProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileDamagePolicy == ESL_ProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

	if (AActor* MyOwner = GetOwner())
	{
		ProjectileCollisionBox->IgnoreActorWhenMoving(MyOwner, true);
		
		if (USL_PawnCombatComponent* CombatComp = USL_FunctionLibrary::NativeGetPawnCombatComponentFromActor(MyOwner))
		{
			if (ASL_WeaponBase* EquippedWeapon = CombatComp->GetCharacterCurrentEquippedWeapon())
			{
				ProjectileCollisionBox->IgnoreActorWhenMoving(EquippedWeapon, true);
			}
		}
	}

	if (APawn* MyInstigator = GetInstigator())
	{
		ProjectileCollisionBox->IgnoreActorWhenMoving(MyInstigator, true);
	}

	if (ProjectileSpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileSpawnSound, GetActorLocation());
	}

	if (ProjectileFlyingSound)
	{
		ProjectileFlyingAudioComponent = UGameplayStatics::SpawnSoundAttached(ProjectileFlyingSound, GetRootComponent());
	}

	if (ProjectileMuzzleFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileMuzzleFX, GetActorLocation(), GetActorForwardVector().Rotation());
	}
}

void ASL_ProjectileBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ProjectileFlyingAudioComponent)
	{
		ProjectileFlyingAudioComponent->Stop();
		ProjectileFlyingAudioComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ASL_ProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnSpawnProjectileHitVFX(Hit.ImpactPoint);
	
	APawn* HitPawn = Cast<APawn>(OtherActor);
	
	if (!HitPawn || !USL_FunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		Destroy();
		return;
	}
	
	bool bIsValidBlock = false;
	const bool bIsPlayerBlocking = USL_FunctionLibrary::NativeDoesActorHaveTag(HitPawn, SL_GameplayTags::Player_Status_Blocking);
	
	if (bIsPlayerBlocking)
	{
		bIsValidBlock = USL_FunctionLibrary::IsValidBlock(this, HitPawn);
	}
	
	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;
	
	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitPawn,
			SL_GameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else
	{
		HandleApplyProjectileDamage(HitPawn, Data);
	}
	
	Destroy();
}

void ASL_ProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor)) return;
	
	OverlappedActors.AddUnique(OtherActor);
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;
		
		if (USL_FunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			HandleApplyProjectileDamage(HitPawn, Data);
		}
	}
}

void ASL_ProjectileBase::HandleApplyProjectileDamage(TObjectPtr<APawn> InHitPawn, const FGameplayEventData& InPayLoad)
{
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());
	
	const bool bWasApplied = USL_FunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);
	
	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			SL_GameplayTags::Shared_Event_HitReact,
			InPayLoad
		);
	}
}

void ASL_ProjectileBase::NativeOnSpawnProjectileHitVFX(const FVector& HitLocation)
{
	if (ProjectileImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileImpactSound, HitLocation);
	}

	if (ProjectileImpactEffects)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ProjectileImpactEffects,
			HitLocation
		);
	}
}

void ASL_ProjectileBase::BP_OnSpawnProjectileHitVFX_Implementation(const FVector& HitLocation)
{
	NativeOnSpawnProjectileHitVFX(HitLocation);
}