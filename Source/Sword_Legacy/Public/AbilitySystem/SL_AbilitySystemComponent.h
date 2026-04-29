#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PawnTypes/SL_PawnStructTypes.h"
#include "SL_AbilitySystemComponent.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Ability", meta = (ApplyLevel = "1"))
	void GrantPlayerWeaponAbilities(const TArray<FSL_PlayerAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Ability")
	void RemoveGrantedPlayerWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);
};