#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "SL_ProjectileBase.generated.h"

struct FGameplayEventData;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UBoxComponent;
class USoundBase;
class UNiagaraSystem;

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
	
	void NativeOnSpawnProjectileHitVFX(const FVector& HitLocation);
	
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
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile", meta = (DisplayName = "On Spawn Projectile Hit VFX"))
	void BP_OnSpawnProjectileHitVFX(const FVector& HitLocation);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Effects")
	TObjectPtr<USoundBase> ProjectileImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Effects")
	TObjectPtr<UNiagaraSystem> ProjectileImpactEffects;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UBoxComponent> ProjectileCollisionBox;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	ESL_ProjectileDamagePolicy ProjectileDamagePolicy = ESL_ProjectileDamagePolicy::OnHit;
	
	UPROPERTY(BlueprintReadOnly, Category = "Projectile", meta = (ExposeOnSpawn = "true"))
	FGameplayEffectSpecHandle ProjectileDamageEffectSpecHandle;
	
private:
	void HandleApplyProjectileDamage(TObjectPtr<APawn> InHitPawn, const FGameplayEventData& InPayLoad);
};