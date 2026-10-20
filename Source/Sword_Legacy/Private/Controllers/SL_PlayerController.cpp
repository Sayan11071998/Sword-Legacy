#include "Controllers/SL_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Components/Widget.h"
#include "GameModes/SL_GameModeBase.h"
#include "GameModes/SL_MainMenuGameMode.h"
#include "Subsystems/SL_UISubsystem.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/WorldSettings.h"
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

void ASL_PlayerController::OnLoadingScreenActivated_Implementation()
{
	bHasHandledLoadingScreenDeactivation = false;
}

void ASL_PlayerController::OnLoadingScreenDeactivated_Implementation()
{
	if (bHasHandledLoadingScreenDeactivation) return;
	if (!IsLocalPlayerController()) return;

	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown) return;

	if (World->GetAuthGameMode<ASL_MainMenuGameMode>())
	{
		bHasHandledLoadingScreenDeactivation = true;
		SetupFrontendUI();
		
		return;
	}

	if (World->GetAuthGameMode<ASL_GameModeBase>())
	{
		bHasHandledLoadingScreenDeactivation = true;
		
		SetupGameplayUI();
	}
}

void ASL_PlayerController::EnsurePrimaryLayoutCreated()
{
	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	
	if (!UISubsystem || UISubsystem->HasPrimaryLayout()) return;
	if (!PrimaryLayoutWidgetClass) return;

	USL_Widget_PrimaryLayout* CreatedPrimaryLayout = CreateWidget<USL_Widget_PrimaryLayout>(this, PrimaryLayoutWidgetClass);
	if (!CreatedPrimaryLayout) return;

	CreatedPrimaryLayout->AddToViewport();
	UISubsystem->RegisterCreatedPrimaryLayoutWidget(CreatedPrimaryLayout);
}

void ASL_PlayerController::ApplyGameplayInputConfig()
{
	if (!IsLocalPlayerController()) return;

	ResetIgnoreInputFlags();

	if (UWorld* World = GetWorld())
	{
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->TimeDilation = 1.f;
			WorldSettings->CinematicTimeDilation = 1.f;
			WorldSettings->DemoPlayTimeDilation = 1.f;
			WorldSettings->SetTimeDilation(1.f);
		}
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>())
		{
			ActionRouter->SetActiveUIInputConfig(
				FUIInputConfig(
					ECommonInputMode::Game,
					EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown
				),
				this
			);
		}
		else
		{
			SetInputMode(FInputModeGameOnly());
		}
	}

	SetShowMouseCursor(false);

	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->EnableInput(this);
	}
}

void ASL_PlayerController::SetupGameplayUI()
{
	EnsurePrimaryLayoutCreated();
	
	ApplyGameplayInputConfig();
}

void ASL_PlayerController::ShowGameLoseScreen()
{
	EnsurePrimaryLayoutCreated();

	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	
	if (!UISubsystem || !UISubsystem->HasPrimaryLayout()) return;

	const TSoftClassPtr<USL_Widget_Activatable_Base> LoseScreenClass = USL_FunctionLibrary::GetGameSoftWidgetClassByTag(SL_GameplayTags::UI_Widget_GameLoseScreen);
	
	if (LoseScreenClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Lose Screen is not assigned in Game UI Settings."));
		
		return;
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>())
		{
			ActionRouter->SetActiveUIInputConfig(
				FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture),
				this);
		}
	}

	SetShowMouseCursor(true);

	TWeakObjectPtr<ASL_PlayerController> WeakThis(this);
	
	UISubsystem->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_GameMenu,
		LoseScreenClass,
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

void ASL_PlayerController::SetupFrontendUI()
{
	EnsurePrimaryLayoutCreated();

	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	if (!UISubsystem) return;

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