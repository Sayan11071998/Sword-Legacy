#include "Utilities/SL_FunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Interfaces/SL_PawnCombatInterface.h"

TObjectPtr<USL_AbilitySystemComponent> USL_FunctionLibrary::NativeGetASCFromActor(TObjectPtr<AActor> InActor)
{
	check(InActor);
	return CastChecked<USL_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

bool USL_FunctionLibrary::NativeDoesActorHaveTag(TObjectPtr<AActor> InActor, FGameplayTag TagToCheck)
{
	USL_AbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);
	return ASC->HasMatchingGameplayTag(TagToCheck);
}

TObjectPtr<USL_PawnCombatComponent> USL_FunctionLibrary::NativeGetPawnCombatComponentFromActor(
	TObjectPtr<AActor> InActor)
{
	check(InActor);
	
	if (ISL_PawnCombatInterface* PawnCombatInterface = Cast<ISL_PawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}
	
	return nullptr;
}

void USL_FunctionLibrary::AddGameplayToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	USL_AbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);
	
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	USL_AbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);
	
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

void USL_FunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, ESL_ConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? ESL_ConfirmType::Yes : ESL_ConfirmType::No;
}

USL_PawnCombatComponent* USL_FunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	ESL_ValidType& OutValidType)
{
	USL_PawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);
	
	OutValidType = CombatComponent ? ESL_ValidType::Valid : ESL_ValidType::Invalid;
	
	return CombatComponent;
}