#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SL_WeaponBase.generated.h"

class UBoxComponent;

UCLASS()
class SWORD_LEGACY_API ASL_WeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASL_WeaponBase();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UBoxComponent> WeaponCollisionBox;
	
public:
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponCollisionBox() { return WeaponCollisionBox; }
};