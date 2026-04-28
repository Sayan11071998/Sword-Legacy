#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SL_FunctionLibrary.generated.h"

class USL_AbilitySystemComponent;

UENUM()
enum class ESL_ConfirmType : uint8
{
	Yes,
	No
};

UCLASS()
class SWORD_LEGACY_API USL_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static TObjectPtr<USL_AbilitySystemComponent> NativeGetASCFromActor(TObjectPtr<AActor> InActor);
	static bool NativeDoesActorHaveTag(TObjectPtr<AActor> InActor, FGameplayTag TagToCheck);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library")
	static void AddGameplayToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library")
	static void RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|Function Library", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, ESL_ConfirmType& OutConfirmType);
};