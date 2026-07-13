#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_GameplayAbility.generated.h"

class USL_AbilitySystemComponent;
class USL_PawnCombatComponent;

UENUM(BlueprintType)
enum class ESL_AbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UCLASS()
class SWORD_LEGACY_API USL_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	// ~ End UGameplayAbility Interface
	
	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(TObjectPtr<AActor> TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);
	
	UFUNCTION(BlueprintPure, Category = "Pawn|Ability")
	USL_PawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;
	
	UFUNCTION(BlueprintPure, Category = "Pawn|Ability")
	USL_AbilitySystemComponent* GetPawnAbilitySystemComponentFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Ability", meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, ESL_SuccessType& OutSuccessType);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Ability")
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults);
	
	UPROPERTY(EditDefaultsOnly, Category = "CharacterAbility")
	ESL_AbilityActivationPolicy AbilityActivationPolicy = ESL_AbilityActivationPolicy::OnTriggered;
};