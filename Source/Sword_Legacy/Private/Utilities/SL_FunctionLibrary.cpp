#include "Utilities/SL_FunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"

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

void USL_FunctionLibrary::AddGameplayToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	USL_AbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);
	
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void USL_FunctionLibrary::RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
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