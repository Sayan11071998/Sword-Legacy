#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/SL_PawnCombatInterface.h"
#include "Interfaces/SL_PawnUIInterface.h"
#include "SL_BaseCharacter.generated.h"

class USL_DataAsset_StartupData_Base;
class USL_AttributeSet;
class USL_AbilitySystemComponent;
class UMotionWarpingComponent;

UCLASS()
class SWORD_LEGACY_API ASL_BaseCharacter : public ACharacter, public IAbilitySystemInterface, public ISL_PawnCombatInterface, public ISL_PawnUIInterface
{
	GENERATED_BODY()

public:
	ASL_BaseCharacter();
	
	// ~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ End IAbilitySystemInterface Interface
	
	// ~ Begin ISL_PawnCombatInterface Interface
	virtual TObjectPtr<USL_PawnCombatComponent> GetPawnCombatComponent() const override;
	// ~ End ISL_PawnCombatInterface Interface
	
	// ~ Begin ISL_PawnUIInterface Interface
	virtual TObjectPtr<USL_PawnUIComponent> GetPawnUIComponent() const override;
	// ~ End ISL_PawnUIInterface Interface
	
protected:
	// ~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<USL_AbilitySystemComponent> CharacterAbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<USL_AttributeSet> CharacterAttributeSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarping")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<USL_DataAsset_StartupData_Base> CharacterStartupData;
	
public:
	FORCEINLINE TObjectPtr<USL_AbilitySystemComponent> GetCharacterAbilitySystemComponent() const { return CharacterAbilitySystemComponent; }
	FORCEINLINE TObjectPtr<USL_AttributeSet> GetCharacterAttributeSet() const { return CharacterAttributeSet; }
};