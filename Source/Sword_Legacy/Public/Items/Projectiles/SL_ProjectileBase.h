#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SL_ProjectileBase.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;
class UBoxComponent;

UENUM(BlueprintType)
enum class ESL_ProjectileDamagePolicy : uint8
{
	OnHit,
	OnBeginOverlap
};

UCLASS()
class SWORD_LEGACY_API ASL_ProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASL_ProjectileBase();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnProjectileHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit	
	);
	
	UFUNCTION()
	virtual void OnProjectileBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult	
	);
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UBoxComponent> ProjectileCollisionBox;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	ESL_ProjectileDamagePolicy ProjectileDamagePolicy = ESL_ProjectileDamagePolicy::OnHit;
};