#include "Items/Collectables/SL_PickupBase.h"
#include "Components/SphereComponent.h"

ASL_PickupBase::ASL_PickupBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	PickupCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollisionSphere"));
	SetRootComponent(PickupCollisionSphere);
	PickupCollisionSphere->InitSphereRadius(50.f);
	
	PickupCollisionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASL_PickupBase::OnPickupCollisionSphereBeginOverlap);
}

void ASL_PickupBase::OnPickupCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ }