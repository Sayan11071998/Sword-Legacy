#include "Utilities/SL_FunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "UI/Credits/SL_CreditTypes.h"
#include "Engine/DataTable.h"
#include "Interfaces/SL_PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_CountdownAction.h"
#include "Settings/SL_DeveloperSettings.h"
#include "Settings/SL_LevelDeveloperSettings.h"
#include "Settings/SL_GameUserSettings.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/SL_LoadingScreenSubsystem.h"

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

int32 USL_FunctionLibrary::GetCurrentGameDifficultyAbilityLevel()
{
	if (const USL_GameUserSettings* GameUserSettings = USL_GameUserSettings::Get())
	{
		return GameUserSettings->GetCurrentGameDifficultyAsAbilityLevel();
	}

	return 2;
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
	float& OutAngleDifference, float FrontArcHalfAngle, float SideArcBoundaryAngle)
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
	
	if (OutAngleDifference >= -FrontArcHalfAngle && OutAngleDifference <= FrontArcHalfAngle)
	{
		return SL_GameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngleDifference < -FrontArcHalfAngle && OutAngleDifference >= -SideArcBoundaryAngle)
	{
		return SL_GameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngleDifference < -SideArcBoundaryAngle || OutAngleDifference > SideArcBoundaryAngle)
	{
		return SL_GameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference > FrontArcHalfAngle && OutAngleDifference <= SideArcBoundaryAngle)
	{
		return SL_GameplayTags::Shared_Status_HitReact_Right;
	}
	
	return SL_GameplayTags::Shared_Status_HitReact_Front;
}

bool USL_FunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender, float BlockFacingDotThreshold)
{
	check(InAttacker && InDefender);
	
	const float DotResult = FVector::DotProduct(
		InAttacker->GetActorForwardVector(), 
		InDefender->GetActorForwardVector()
	);
	
	return DotResult < BlockFacingDotThreshold;
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

TSoftClassPtr<USL_Widget_Activatable_Base> USL_FunctionLibrary::GetGameSoftWidgetClassByTag(UPARAM(meta = (Categories = "UI.Widget")) FGameplayTag InWidgetTag)
{
	const USL_DeveloperSettings* GameDeveloperSettings = GetDefault<USL_DeveloperSettings>();
	
	checkf(GameDeveloperSettings->GameWidgetMap.Contains(InWidgetTag), TEXT("Could not find the corresponding widget under the tag %s"), *InWidgetTag.ToString());
	
	return GameDeveloperSettings->GameWidgetMap.FindRef(InWidgetTag);
}

TSoftObjectPtr<UTexture2D> USL_FunctionLibrary::GetOptionsSoftImageByTag(FGameplayTag InImageTag)
{
	const USL_DeveloperSettings* GameDeveloperSettings = GetDefault<USL_DeveloperSettings>();
	
	checkf(GameDeveloperSettings->OptionsScreenSoftImageMap.Contains(InImageTag), TEXT("Could not find an image associated with the tag %s"), *InImageTag.ToString());
	
	return GameDeveloperSettings->OptionsScreenSoftImageMap.FindRef(InImageTag);
}

namespace
{
	FString BuildCreditsDisplayString(const TArray<FSL_CreditTableRow>& Rows)
	{
		FString Result = TEXT("Credits\r\n\r\n");
		FString CurrentSection;

		for (const FSL_CreditTableRow& Row : Rows)
		{
			if (Row.PersonName.IsEmpty()) continue;

			const FString Section = Row.Section.ToString();
			if (!Section.Equals(CurrentSection, ESearchCase::CaseSensitive))
			{
				if (!CurrentSection.IsEmpty())
				{
					Result += TEXT("\r\n");
				}

				CurrentSection = Section;

				if (!Section.IsEmpty())
				{
					Result += Section + TEXT("\r\n\r\n");
				}
			}

			if (!Row.Role.IsEmpty())
			{
				Result += Row.Role.ToString() + TEXT("\r\n");
			}

			Result += Row.PersonName.ToString() + TEXT("\r\n\r\n");
		}

		return Result;
	}
}

FString USL_FunctionLibrary::GetCreditsDisplayString()
{
	const USL_DeveloperSettings* GameDeveloperSettings = GetDefault<USL_DeveloperSettings>();
	const UDataTable* CreditsTable = GameDeveloperSettings && !GameDeveloperSettings->CreditsTable.IsNull()
		? GameDeveloperSettings->CreditsTable.LoadSynchronous()
		: nullptr;

	if (!CreditsTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Credits Table is not assigned on Game UI Settings."));
		return FString();
	}

	TArray<FSL_CreditTableRow> Rows;
	CreditsTable->ForeachRow<FSL_CreditTableRow>(
		TEXT("GetCreditsDisplayString"),
		[&Rows](const FName& /*RowName*/, const FSL_CreditTableRow& Row)
		{
			Rows.Add(Row);
		}
	);

	return BuildCreditsDisplayString(Rows);
}

void USL_FunctionLibrary::OpenLevelWithLoadingScreen(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level, bool bAbsolute, FString Options)
{
	if (!WorldContextObject || Level.IsNull()) return;

	if (const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (USL_LoadingScreenSubsystem* LoadingScreenSubsystem = GameInstance->GetSubsystem<USL_LoadingScreenSubsystem>())
			{
				LoadingScreenSubsystem->ShowLoadingScreen(TEXT("Loading level"));
			}
		}
	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(WorldContextObject, Level, bAbsolute, Options);
}

TSoftObjectPtr<UWorld> USL_FunctionLibrary::GetGameLevelByTag(FGameplayTag InLevelTag)
{
	const USL_LevelDeveloperSettings* LevelDeveloperSettings = GetDefault<USL_LevelDeveloperSettings>();
	
	if (!LevelDeveloperSettings || !LevelDeveloperSettings->GameLevelMap.Contains(InLevelTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find a level under the tag %s"), *InLevelTag.ToString());
		
		return TSoftObjectPtr<UWorld>();
	}

	return LevelDeveloperSettings->GameLevelMap.FindRef(InLevelTag);
}

bool USL_FunctionLibrary::IsCurrentLevelByTag(const UObject* WorldContextObject, FGameplayTag InLevelTag)
{
	if (!WorldContextObject || !InLevelTag.IsValid()) return false;

	const TSoftObjectPtr<UWorld> Level = GetGameLevelByTag(InLevelTag);
	if (Level.IsNull()) return false;

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(WorldContextObject, true);
	
	return CurrentLevelName == Level.ToSoftObjectPath().GetAssetName();
}

void USL_FunctionLibrary::OpenLevelByTagWithLoadingScreen(const UObject* WorldContextObject, FGameplayTag InLevelTag, bool bAbsolute, FString Options)
{
	const TSoftObjectPtr<UWorld> Level = GetGameLevelByTag(InLevelTag);
	
	if (Level.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Level tag %s is not assigned in Game Level Settings."), *InLevelTag.ToString());
		
		return;
	}

	OpenLevelWithLoadingScreen(WorldContextObject, Level, bAbsolute, Options);
}

void USL_FunctionLibrary::OpenLevelByNameWithLoadingScreen(const UObject* WorldContextObject, FName LevelName, bool bAbsolute, FString Options)
{
	if (!WorldContextObject || LevelName.IsNone()) return;

	if (const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (USL_LoadingScreenSubsystem* LoadingScreenSubsystem = GameInstance->GetSubsystem<USL_LoadingScreenSubsystem>())
			{
				LoadingScreenSubsystem->ShowLoadingScreen(TEXT("Loading level"));
			}
		}
	}

	UGameplayStatics::OpenLevel(WorldContextObject, LevelName, bAbsolute, Options);
}