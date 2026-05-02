#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SL_PawnExtensionComponentBase.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_PawnCombatComponent.generated.h"

struct FGameplayTag;
class ASL_WeaponBase;

UCLASS()
class SWORD_LEGACY_API USL_PawnCombatComponent : public USL_PawnExtensionComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, ASL_WeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	ASL_WeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	ASL_WeaponBase* GetCharacterCurrentEquippedWeapon() const;
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Combat")
	void ToggleWeaponCollision(bool bShouldEnable, ESL_ToggleDamageType ToggleDamageType = ESL_ToggleDamageType::CurrentEquippedWeapon);
	
	UPROPERTY(BlueprintReadWrite, Category = "Pawn|Combat")
	FGameplayTag CurrentEquippedWeaponTag;
	
protected:
	TArray<TObjectPtr<AActor>> OverlappedActors;
	
private:
	UPROPERTY()
	TMap<FGameplayTag, ASL_WeaponBase*> CharacterCarriedWeaponMap;
};