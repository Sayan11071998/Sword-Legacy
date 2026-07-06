#pragma once

#include "CoreMinimal.h"
#include "Components/UI/SL_PawnUIComponent.h"
#include "GameplayTagContainer.h"
#include "SL_PlayerUIComponent.generated.h"

class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftWeaponIcon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityIconSlotUpdatedDelegate, FGameplayTag, AbilityInputTag, TSoftObjectPtr<UMaterialInterface>, SoftAbilityIconMaterial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCooldownBeginDelegate, FGameplayTag, AbilityInputTag, float, TotalCooldownTime, float, RemainingCooldownTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoneInteractedDelegate, bool, bShouldDisplayIconInputKey);

UCLASS()
class SWORD_LEGACY_API USL_PlayerUIComponent : public USL_PawnUIComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void RegisterPlayerDrawnWidget(UUserWidget* InWidgetToRegister);
	
	UFUNCTION(BlueprintCallable)
	void RemovePlayerDrawnWidgetsIfAny();
	
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentRageChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAbilityIconSlotUpdatedDelegate OnAbilityIconSlotUpdated;
	
	UPROPERTY(BlueprintAssignable)
	FOnAbilityCooldownBeginDelegate OnAbilityCooldownBegin;
	
	UPROPERTY(BlueprintAssignable)
	FOnStoneInteractedDelegate OnStoneInteracted;

private:
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> PlayerDrawnWidgets;
};