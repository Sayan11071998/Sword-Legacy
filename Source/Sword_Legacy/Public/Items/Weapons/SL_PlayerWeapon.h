#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/SL_WeaponBase.h"
#include "PawnTypes/SL_PawnStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "SL_PlayerWeapon.generated.h"

UCLASS()
class SWORD_LEGACY_API ASL_PlayerWeapon : public ASL_WeaponBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FSL_PlayerWeaponData PlayerWeaponData;
	
	UFUNCTION(BlueprintCallable)
	void AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);
	
	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle>  GetGrantedAbilitySpecHandles() const;
	
private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};