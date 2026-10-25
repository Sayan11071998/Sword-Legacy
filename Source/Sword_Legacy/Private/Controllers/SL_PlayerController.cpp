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
#include "Settings/SL_DeveloperSettings.h"
#include "Subsystems/SL_LoadingScreenSubsystem.h"
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

void ASL_PlayerController::ShowGameEnding()
{
	EnsurePrimaryLayoutCreated();

	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	if (!UISubsystem || !UISubsystem->HasPrimaryLayout()) return;

	const USL_DeveloperSettings* DeveloperSettings = GetDefault<USL_DeveloperSettings>();
	
	const TSoftClassPtr<USL_Widget_Activatable_Base> ThankYouScreenClass = DeveloperSettings
		? DeveloperSettings->GameWidgetMap.FindRef(SL_GameplayTags::UI_Widget_GameThankYouScreen)
		: TSoftClassPtr<USL_Widget_Activatable_Base>();

	if (ThankYouScreenClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Thank You Screen is not assigned in Game UI Settings."));
		
		return;
	}

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	SetShowMouseCursor(false);

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>())
		{
			ActionRouter->SetActiveUIInputConfig(
				FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture),
				this
			);
		}
	}

	TWeakObjectPtr<ASL_PlayerController> WeakThis(this);

	UISubsystem->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_GameMenu,
		ThankYouScreenClass,
		[WeakThis](EAsyncPushWidgetState InPushState, TObjectPtr<USL_Widget_Activatable_Base> PushedWidget)
		{
			if (!PushedWidget) return;

			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				if (ASL_PlayerController* PlayerController = WeakThis.Get())
				{
					PushedWidget->SetOwningPlayer(PlayerController);
				}
			}
		}
	);
}

void ASL_PlayerController::ShowEndingCredits(USL_Widget_Activatable_Base* ThankYouScreen)
{
	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	if (!UISubsystem || !UISubsystem->HasPrimaryLayout()) return;

	const USL_DeveloperSettings* DeveloperSettings = GetDefault<USL_DeveloperSettings>();
	
	const TSoftClassPtr<USL_Widget_Activatable_Base> CreditScreenClass = DeveloperSettings
		? DeveloperSettings->GameWidgetMap.FindRef(SL_GameplayTags::UI_Widget_EndingCreditScreen)
		: TSoftClassPtr<USL_Widget_Activatable_Base>();

	if (CreditScreenClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ending Credit Screen is not assigned in Game UI Settings."));
		return;
	}

	TWeakObjectPtr<ASL_PlayerController> WeakThis(this);
	TWeakObjectPtr<USL_Widget_Activatable_Base> WeakThankYouScreen(ThankYouScreen);

	UISubsystem->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_GameMenu,
		CreditScreenClass,
		[WeakThis, WeakThankYouScreen](EAsyncPushWidgetState InPushState, TObjectPtr<USL_Widget_Activatable_Base> PushedWidget)
		{
			if (!PushedWidget) return;

			ASL_PlayerController* PlayerController = WeakThis.Get();

			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				if (PlayerController)
				{
					PushedWidget->SetOwningPlayer(PlayerController);
				}
			}
			else if (InPushState == EAsyncPushWidgetState::AfterPush)
			{
				if (!PlayerController) return;

				if (USL_UISubsystem* EndingUISubsystem = USL_UISubsystem::Get(PlayerController))
				{
					EndingUISubsystem->RemoveWidgetFromStack(SL_GameplayTags::UI_WidgetStack_GameMenu, WeakThankYouScreen.Get());
				}

				PlayerController->SetShowMouseCursor(false);

				if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
				{
					if (UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>())
					{
						ActionRouter->SetActiveUIInputConfig(
							FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture),
							PlayerController
						);
					}
				}
			}
		}
	);
}

void ASL_PlayerController::ShowEndingPressAnyKey(USL_Widget_Activatable_Base* CreditScreen)
{
	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	if (!UISubsystem || !UISubsystem->HasPrimaryLayout()) return;

	const USL_DeveloperSettings* DeveloperSettings = GetDefault<USL_DeveloperSettings>();
	
	const TSoftClassPtr<USL_Widget_Activatable_Base> PressAnyKeyClass = DeveloperSettings
		? DeveloperSettings->GameWidgetMap.FindRef(SL_GameplayTags::UI_Widget_EndingPressAnyKeyScreen)
		: TSoftClassPtr<USL_Widget_Activatable_Base>();

	if (PressAnyKeyClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ending Press Any Key Screen is not assigned in Game UI Settings."));
		
		return;
	}

	TWeakObjectPtr<ASL_PlayerController> WeakThis(this);
	TWeakObjectPtr<USL_Widget_Activatable_Base> WeakCreditScreen(CreditScreen);

	UISubsystem->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_GameMenu,
		PressAnyKeyClass,
		[WeakThis, WeakCreditScreen](EAsyncPushWidgetState InPushState, TObjectPtr<USL_Widget_Activatable_Base> PushedWidget)
		{
			if (!PushedWidget) return;

			ASL_PlayerController* PlayerController = WeakThis.Get();

			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				if (PlayerController)
				{
					PushedWidget->SetOwningPlayer(PlayerController);
				}
			}
			else if (InPushState == EAsyncPushWidgetState::AfterPush)
			{
				if (!PlayerController) return;

				if (USL_UISubsystem* EndingUISubsystem = USL_UISubsystem::Get(PlayerController))
				{
					EndingUISubsystem->RemoveWidgetFromStack(SL_GameplayTags::UI_WidgetStack_GameMenu, WeakCreditScreen.Get());
				}

				PlayerController->SetShowMouseCursor(true);

				if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
				{
					WidgetToFocus->SetFocus();
				}
			}
		}
	);
}

void ASL_PlayerController::ToggleGamePause()
{
	if (!IsLocalPlayerController() || bIsGamePaused) return;

	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown) return;

	const ASL_GameModeBase* GameMode = World->GetAuthGameMode<ASL_GameModeBase>();
	if (!GameMode || GameMode->HasPlayerLost() || GameMode->HasLevelCleared()) return;

	if (APawn* ControlledPawn = GetPawn())
	{
		if (USL_FunctionLibrary::NativeDoesActorHaveTag(ControlledPawn, SL_GameplayTags::Shared_Status_Death)) return;
	}

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (const USL_LoadingScreenSubsystem* LoadingScreen = GameInstance->GetSubsystem<USL_LoadingScreenSubsystem>())
		{
			if (!LoadingScreen->IsGameplayReady()) return;
		}
	}

	EnsurePrimaryLayoutCreated();

	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	if (!UISubsystem || !UISubsystem->HasPrimaryLayout()) return;

	const USL_DeveloperSettings* DeveloperSettings = GetDefault<USL_DeveloperSettings>();
	const TSoftClassPtr<USL_Widget_Activatable_Base> PauseScreenClass = DeveloperSettings
		? DeveloperSettings->GameWidgetMap.FindRef(SL_GameplayTags::UI_Widget_GamePauseScreen)
		: TSoftClassPtr<USL_Widget_Activatable_Base>();

	if (PauseScreenClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Pause Screen is not assigned in Game UI Settings."));
		
		return;
	}

	TWeakObjectPtr<ASL_PlayerController> WeakThis(this);

	UISubsystem->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_GameMenu,
		PauseScreenClass,
		[WeakThis](EAsyncPushWidgetState InPushState, TObjectPtr<USL_Widget_Activatable_Base> PushedWidget)
		{
			if (!PushedWidget) return;

			ASL_PlayerController* PlayerController = WeakThis.Get();
			if (!PlayerController) return;

			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				PushedWidget->SetOwningPlayer(PlayerController);
			}
			else if (InPushState == EAsyncPushWidgetState::AfterPush)
			{
				PlayerController->bIsGamePaused = true;

				if (UWorld* PausedWorld = PlayerController->GetWorld())
				{
					if (AWorldSettings* WorldSettings = PausedWorld->GetWorldSettings())
					{
						WorldSettings->SetTimeDilation(0.f);
					}
				}

				PlayerController->SetShowMouseCursor(true);

				if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
				{
					WidgetToFocus->SetFocus();
				}
			}
		}
	);
}

void ASL_PlayerController::EndGamePause()
{
	if (!bIsGamePaused) return;

	bIsGamePaused = false;

	if (UWorld* World = GetWorld())
	{
		if (!World->bIsTearingDown)
		{
			if (AWorldSettings* WorldSettings = World->GetWorldSettings())
			{
				WorldSettings->TimeDilation = 1.f;
				WorldSettings->CinematicTimeDilation = 1.f;
				WorldSettings->DemoPlayTimeDilation = 1.f;
				WorldSettings->SetTimeDilation(1.f);
			}
		}
	}

	ApplyGameplayInputConfig();
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