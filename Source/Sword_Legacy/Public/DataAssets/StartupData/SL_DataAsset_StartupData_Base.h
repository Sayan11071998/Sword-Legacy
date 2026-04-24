#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SL_DataAsset_StartupData_Base.generated.h"

class USL_AbilitySystemComponent;
class USL_GameplayAbility;

UCLASS()
class SWORD_LEGACY_API USL_DataAsset_StartupData_Base : public UDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void GiveToAbilitySystemComponent(TObjectPtr<USL_AbilitySystemComponent> InASCToGive, int32 ApplyLevel = 1);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Startup Data")
	TArray<TSubclassOf<USL_GameplayAbility>> ActivateOnGivenAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Startup Data")
	TArray<TSubclassOf<USL_GameplayAbility>> ReactiveAbilities;
	
	void GrantAbilities(
		const TArray<TSubclassOf<USL_GameplayAbility>>& InAbilitiesToGive,
		TObjectPtr<USL_AbilitySystemComponent> InASCToGive,
		int32 ApplyLevel = 1
	);
};