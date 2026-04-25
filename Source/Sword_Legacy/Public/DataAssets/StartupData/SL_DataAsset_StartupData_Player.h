#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DataAssets/StartupData/SL_DataAsset_StartupData_Base.h"
#include "SL_DataAsset_StartupData_Player.generated.h"

USTRUCT(BlueprintType)
struct FSL_PlayerAbilitySet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<USL_GameplayAbility> AbilityToGrant;
	
	bool IsValid() const;
};

UCLASS()
class SWORD_LEGACY_API USL_DataAsset_StartupData_Player : public USL_DataAsset_StartupData_Base
{
	GENERATED_BODY()
	
public:
	// ~ Begin USL_DataAsset_StartupData_Base Interface
	virtual void GiveToAbilitySystemComponent(TObjectPtr<USL_AbilitySystemComponent> InASCToGive, int32 ApplyLevel = 1) override;
	// ~ End USL_DataAsset_StartupData_Base Interface
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Startup Data", meta = (Categories = "InputTag"))
	TArray<FSL_PlayerAbilitySet> PlayerStartupAbilitySets;
};