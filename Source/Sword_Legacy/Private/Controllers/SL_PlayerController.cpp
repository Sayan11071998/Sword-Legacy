#include "Controllers/SL_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Components/Widget.h"
#include "GameModes/SL_MainMenuGameMode.h"
#include "Subsystems/SL_UISubsystem.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "UI/Widgets/SL_Widget_PrimaryLayout.h"

ASL_PlayerController::ASL_PlayerController()
{
	PlayerTeamID = FGenericTeamId(0);
}

FGenericTeamId ASL_PlayerController::GetGenericTeamId() const
{
	return PlayerTeamID;
}

void ASL_PlayerController::OnLoadingScreenDeactivated_Implementation()
{
	if (bHasHandledLoadingScreenDeactivation) return;
	if (!IsLocalPlayerController()) return;

	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown) return;
	if (!World->GetAuthGameMode<ASL_MainMenuGameMode>()) return;

	bHasHandledLoadingScreenDeactivation = true;

	SetupFrontendUI();
}

void ASL_PlayerController::SetupFrontendUI()
{
	if (!PrimaryLayoutWidgetClass) return;

	USL_Widget_PrimaryLayout* CreatedPrimaryLayout = CreateWidget<USL_Widget_PrimaryLayout>(this, PrimaryLayoutWidgetClass);
	if (!CreatedPrimaryLayout) return;

	CreatedPrimaryLayout->AddToViewport();

	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	if (!UISubsystem) return;

	UISubsystem->RegisterCreatedPrimaryLayoutWidget(CreatedPrimaryLayout);

	const TSoftClassPtr<USL_Widget_Activatable_Base> PressAnyKeyWidgetClass = USL_FunctionLibrary::GetGameSoftWidgetClassByTag(SL_GameplayTags::UI_Widget_PressAnyKeyScreen);
	if (PressAnyKeyWidgetClass.IsNull()) return;

	TWeakObjectPtr<ASL_PlayerController> WeakThis(this);
	
	UISubsystem->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_Frontend,
		PressAnyKeyWidgetClass,
		[WeakThis](EAsyncPushWidgetState InPushState, TObjectPtr<USL_Widget_Activatable_Base> PushedWidget)
		{
			if (!PushedWidget) return;

			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				if (ASL_PlayerController* Controller = WeakThis.Get())
				{
					PushedWidget->SetOwningPlayer(Controller);
				}
			}
			else if (InPushState == EAsyncPushWidgetState::AfterPush)
			{
				if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
				{
					WidgetToFocus->SetFocus();
				}
			}
		}
	);
}

void ASL_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AActor* CurrentViewTarget = GetViewTarget())
	{
		if (CurrentViewTarget != this && CurrentViewTarget != GetPawn())
		{
			SetViewTarget(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ASL_PlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (!aPawn)
	{
		SetViewTargetToDefaultCamera();
	}
}

void ASL_PlayerController::SetViewTargetToDefaultCamera()
{
	if (!GetWorld() || GetWorld()->bIsTearingDown) return;

	TArray<AActor*> FoundCameras;

	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName("Default"), FoundCameras);

	if (!FoundCameras.IsEmpty() && IsValid(FoundCameras[0]))
	{
		SetViewTarget(FoundCameras[0]);
	}
}