#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "SL_AttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class ISL_PawnUIInterface;

UCLASS()
class SWORD_LEGACY_API USL_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	USL_AttributeSet();
	
	// ~ Begin UAttributeSet Interface
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;
	// ~ End UAttributeSet Interface
	
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(USL_AttributeSet, CurrentHealth)
	
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USL_AttributeSet, MaxHealth)
	
	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData CurrentRage;
	ATTRIBUTE_ACCESSORS(USL_AttributeSet, CurrentRage)
	
	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(USL_AttributeSet, MaxRage)
	
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(USL_AttributeSet, AttackPower)
	
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(USL_AttributeSet, DefensePower)
	
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(USL_AttributeSet, DamageTaken)
	
private:
	TWeakInterfacePtr<ISL_PawnUIInterface> CachedPawnUIInterface;
};