#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SL_PawnExtensionComponentBase.h"
#include "SL_PawnCombatComponent.generated.h"

struct FGameplayTag;
class ASL_WeaponBase;

UCLASS()
class SWORD_LEGACY_API USL_PawnCombatComponent : public USL_PawnExtensionComponentBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, ASL_WeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	ASL_WeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	ASL_WeaponBase* GetCharacterCurrentEquippedWeapon() const;
	
	UPROPERTY(BlueprintReadWrite, Category = "Pawn|Combat")
	FGameplayTag CurrentEquippedWeaponTag;
	
private:
	UPROPERTY()
	TMap<FGameplayTag, ASL_WeaponBase*> CharacterCarriedWeaponMap;
};