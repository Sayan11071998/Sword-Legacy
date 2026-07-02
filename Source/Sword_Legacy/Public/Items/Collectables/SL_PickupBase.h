#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SL_PickupBase.generated.h"

class USphereComponent;

UCLASS()
class SWORD_LEGACY_API ASL_PickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASL_PickupBase();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup Interaction")
	TObjectPtr<USphereComponent> PickupCollisionSphere;
	
	UFUNCTION()
	virtual void OnPickupCollisionSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult	
	);
};