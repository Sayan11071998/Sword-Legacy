#include "Utilities/SL_FunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Interfaces/SL_PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utilities/SL_GameplayTags.h"
#include "Utilities/SL_CountdownAction.h"

#include "SL_DebugHelper.h"

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

void USL_FunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
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

bool USL_FunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);
	
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());
	
	if (QueryTeamAgent && TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	
	return false;
}

float USL_FunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag USL_FunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim,
	float& OutAngleDifference)
{
	check(InAttacker && InVictim);
	
	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();
	
	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);
	
	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);
	
	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1.f;
	}
	
	if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
	{
		return SL_GameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
	{
		return SL_GameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngleDifference < -135.f || OutAngleDifference > 135.f)
	{
		return SL_GameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
	{
		return SL_GameplayTags::Shared_Status_HitReact_Right;
	}
	
	return SL_GameplayTags::Shared_Status_HitReact_Front;
}

bool USL_FunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);
	
	const float DotResult = FVector::DotProduct(
		InAttacker->GetActorForwardVector(), 
		InDefender->GetActorForwardVector()
	);
	
	// const FString DebugString = FString::Printf(TEXT("Dot Result: %f %s"), DotResult, DotResult < 0.f ? TEXT("Valid Block") : TEXT("Invalid Block"));
	// Debug::Print(DebugString, DotResult < -0.1f ? FColor::Green : FColor::Red);
	
	return DotResult < -0.1f;
}

bool USL_FunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	USL_AbilitySystemComponent* SourceASC = NativeGetASCFromActor(InInstigator);
	USL_AbilitySystemComponent* TargetASC = NativeGetASCFromActor(InTargetActor);
	
	FActiveGameplayEffectHandle ActiveGameplayEffectHandl = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
	
	return ActiveGameplayEffectHandl.WasSuccessfullyApplied();
}

void USL_FunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
	float& OutRemainingTime, ESL_CountDownActionInput CountdownInput, UPARAM(DisplayName = "Output") ESL_CountDownActionOutput& CountdownOutput,
	FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;
	
	if (GEngine)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}
	
	if (!World) return;
	
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
	FSL_CountdownAction* FoundAction = LatentActionManager.FindExistingAction<FSL_CountdownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
	
	if (CountdownInput == ESL_CountDownActionInput::Start)
	{
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,
				LatentInfo.UUID,
				new FSL_CountdownAction(
					TotalTime,
					UpdateInterval,
					OutRemainingTime,
					CountdownOutput,
					LatentInfo
				)
			);
		}
	}
	
	if (CountdownInput == ESL_CountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancelAction();
		}
	}
}