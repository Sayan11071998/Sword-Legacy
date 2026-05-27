#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_FunctionLibrary.generated.h"

struct FScalableFloat;
class USL_PawnCombatComponent;
class USL_AbilitySystemComponent;

UCLASS()
class SWORD_LEGACY_API USL_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static TObjectPtr<USL_AbilitySystemComponent> NativeGetASCFromActor(TObjectPtr<AActor> InActor);
	static bool NativeDoesActorHaveTag(TObjectPtr<AActor> InActor, FGameplayTag TagToCheck);
	static TObjectPtr<USL_PawnCombatComponent> NativeGetPawnCombatComponentFromActor(TObjectPtr<AActor> InActor);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, ESL_ConfirmType& OutConfirmType);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static  USL_PawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, ESL_ValidType& OutValidType);
	
	UFUNCTION(BlueprintPure, Category = "Pawn|Function Library")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);
	
	UFUNCTION(BlueprintPure, Category = "Pawn|Function Library", meta = (CompactNodeTitle = "Get Value At Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel = 1.f);
	
	UFUNCTION(BlueprintPure, Category = "Pawn|Function Library")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);
	
	UFUNCTION(BlueprintPure, Category = "Pawn|Function Library")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);
};