#pragma once

#include "CoreMinimal.h"
#include "Items/Collectables/SL_PickupBase.h"
#include "SL_StoneBase.generated.h"

class UGameplayEffect;
class USL_AbilitySystemComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class SWORD_LEGACY_API ASL_StoneBase : public ASL_PickupBase
{
	GENERATED_BODY()
	
public:
	void Consume(USL_AbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel);
	
protected:
	// ~ Begin ASL_PickupBase Interface
	virtual void OnPickupCollisionSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult	
	) override;
	// ~ End ASL_PickupBase Interface
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Stone Consumed"))
	void BP_OnStoneConsumed();
	
	UPROPERTY(EditDefaultsOnly, Category = "Stone")
	TSubclassOf<UGameplayEffect> StoneGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Stone")
	TObjectPtr<UNiagaraSystem> ConsumeEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Stone")
	TObjectPtr<USoundBase> ConsumeSoundEffects;
};