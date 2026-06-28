#pragma once

#include "CoreMinimal.h"
#include "Items/Collectables/SL_PickupBase.h"
#include "SL_StoneBase.generated.h"

UCLASS()
class SWORD_LEGACY_API ASL_StoneBase : public ASL_PickupBase
{
	GENERATED_BODY()
	
protected:
	// ~ Begin ASL_PickupBase Interface
	UFUNCTION()
	virtual void OnPickupCollisionSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult	
	) override;
	// ~ End ASL_PickupBase Interface
};