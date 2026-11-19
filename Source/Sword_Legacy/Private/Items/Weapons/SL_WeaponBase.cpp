#include "Items/Weapons/SL_WeaponBase.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Utilities/SL_FunctionLibrary.h"

ASL_WeaponBase::ASL_WeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);

	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	WeaponSkeletalMesh->SetupAttachment(GetRootComponent());
	WeaponSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponSkeletalMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	WeaponSkeletalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponSkeletalMesh->SetComponentTickEnabled(false);
	
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASL_WeaponBase::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ASL_WeaponBase::OnCollisionBoxEndOverlap);
}

void ASL_WeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyActiveWeaponMesh();
}

void ASL_WeaponBase::BeginPlay()
{
	Super::BeginPlay();

	ApplyActiveWeaponMesh();
}

void ASL_WeaponBase::ApplyActiveWeaponMesh()
{
	const bool bUseSkeletalMesh = WeaponSkeletalMesh && WeaponSkeletalMesh->GetSkeletalMeshAsset() != nullptr;

	if (WeaponMesh)
	{
		WeaponMesh->SetVisibility(!bUseSkeletalMesh);
		WeaponMesh->SetHiddenInGame(bUseSkeletalMesh);

		if (bUseSkeletalMesh)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (!WeaponSkeletalMesh) return;

	WeaponSkeletalMesh->SetVisibility(bUseSkeletalMesh);
	WeaponSkeletalMesh->SetHiddenInGame(!bUseSkeletalMesh);

	if (!bUseSkeletalMesh)
	{
		WeaponSkeletalMesh->SetComponentTickEnabled(false);
		return;
	}

	const bool bPlayAnimation = WeaponSkeletalMesh->GetAnimClass() != nullptr;

	WeaponSkeletalMesh->bRenderStatic = !bPlayAnimation;
	WeaponSkeletalMesh->SetComponentTickEnabled(bPlayAnimation);

	if (!bPlayAnimation && WeaponSkeletalMesh->IsRegistered())
	{
		WeaponSkeletalMesh->RefreshBoneTransforms();
		WeaponSkeletalMesh->MarkRenderStateDirty();
	}
}

UMeshComponent* ASL_WeaponBase::GetWeaponMesh() const
{
	if (WeaponSkeletalMesh && WeaponSkeletalMesh->GetSkeletalMeshAsset())
	{
		return WeaponSkeletalMesh;
	}

	return WeaponMesh;
}

void ASL_WeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon: %s"), *GetName());
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (USL_FunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
}

void ASL_WeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instigator as the owning pawn for the weapon: %s"), *GetName());
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (USL_FunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}