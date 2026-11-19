#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "GameFramework/Actor.h"
#include "SL_WeaponBase.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;

DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*)

UCLASS()
class SWORD_LEGACY_API ASL_WeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASL_WeaponBase();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	UMeshComponent* GetWeaponMesh() const;
	
	FOnTargetInteractedDelegate OnWeaponHitTarget;
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
	
protected:
	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult	
	);
	
	void ApplyActiveWeaponMesh();

	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex	
	);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UBoxComponent> WeaponCollisionBox;
	
public:
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponCollisionBox() const { return WeaponCollisionBox; }
};