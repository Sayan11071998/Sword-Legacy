#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_FunctionLibrary.generated.h"

class USL_Widget_Activatable_Base;
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
	static FGameplayTag ComputeHitReactDirectionTag(
		AActor* InAttacker,
		AActor* InVictim,
		float& OutAngleDifference,
		float FrontArcHalfAngle = 45.f,
		float SideArcBoundaryAngle = 135.f
	);
	
	UFUNCTION(BlueprintPure, Category = "Pawn|Function Library")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender, float BlockFacingDotThreshold = -0.1f);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo", ExpandEnumAsExecs = "CountdownInput|CountdownOutput", TotalTime = "1.0", UpdateInterval = "0.1"))
	static void CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime, ESL_CountDownActionInput CountdownInput, UPARAM(DisplayName = "Output") ESL_CountDownActionOutput& CountdownOutput, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library")
	static TSoftClassPtr<USL_Widget_Activatable_Base> GetGameSoftWidgetClassByTag(FGameplayTag InWidgetTag);
};