#include "Subsystems/SL_LoadingScreenSubsystem.h"
#include "PreLoadScreenManager.h"
#include "Subsystems/SL_LoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "GameModes/SL_MainMenuGameMode.h"
#include "Interfaces/SL_LoadingScreenInterface.h"
#include "ShaderCompiler.h"
#include "ShaderPipelineCache.h"

namespace SLLoadingScreen
{
	constexpr int32 MinWorldSettleFrames = 20;
	constexpr float MaxShaderWaitSeconds = 60.f;
}

bool USL_LoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		
		GetDerivedClasses(GetClass(), FoundClasses);
		
		return FoundClasses.IsEmpty();
	}
	
	return false;
}

void USL_LoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &USL_LoadingScreenSubsystem::OnMapPreLoaded);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USL_LoadingScreenSubsystem::OnMapPostLoaded);
	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &USL_LoadingScreenSubsystem::OnWorldBeginTearDown);

	if (const USL_LoadingScreenSettings* LoadingScreenSettings = GetDefault<USL_LoadingScreenSettings>())
	{
		if (!LoadingScreenSettings->SoftLoadingScreenWidgetClass.IsNull())
		{
			LoadingScreenSettings->SoftLoadingScreenWidgetClass.LoadSynchronous();
		}
	}
}

void USL_LoadingScreenSubsystem::Deinitialize()
{
	bIsShuttingDown = true;
	SetTickableTickType(ETickableTickType::Never);
	TryRemoveLoadingScreen();
	CachedCreatedLoadingScreenWidget.Reset();
	CachedLoadingScreenUserWidget = nullptr;

	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	FWorldDelegates::OnWorldBeginTearDown.RemoveAll(this);

	Super::Deinitialize();
}

UWorld* USL_LoadingScreenSubsystem::GetTickableGameObjectWorld() const
{
	if (UGameInstance* OwningGameInstance = GetGameInstance())
	{
		return OwningGameInstance->GetWorld();
	}
	
	return nullptr;
}

void USL_LoadingScreenSubsystem::Tick(float DeltaTime)
{
	TryUpdateLoadingScreen();
}

ETickableTickType USL_LoadingScreenSubsystem::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}
	
	return ETickableTickType::Conditional;
}

bool USL_LoadingScreenSubsystem::IsTickable() const
{
	if (bIsShuttingDown || !GetGameInstance() || !GetGameInstance()->GetGameViewportClient()) return false;

	const UWorld* World = GetGameInstance()->GetWorld();
	
	return !World || !World->bIsTearingDown;
}

bool USL_LoadingScreenSubsystem::IsTickableWhenPaused() const
{
	return true;
}

TStatId USL_LoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USL_LoadingScreenSubsystem, STATGROUP_Tickables);
}

void USL_LoadingScreenSubsystem::ShowLoadingScreen(const FString& Reason)
{
	if (bIsShuttingDown) return;

	bForceShowLoadingScreen = true;
	bHoldGameplay = true;
	bGameplayReady = false;
	
	CurrentLoadingReason = Reason.IsEmpty() ? TEXT("Loading") : Reason;
	
	SetTickableTickType(ETickableTickType::Conditional);
	
	TryUpdateLoadingScreen();
}

void USL_LoadingScreenSubsystem::OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance()) return;

	bIsShuttingDown = false;
	bForceShowLoadingScreen = false;
	bIsCurrentlyLoadingMap = true;
	bHoldGameplay = true;
	bGameplayReady = false;
	WorldSettleFrames = 0;
	ShaderWaitStartTime = -1.f;
	HoldLoadingScreenStartupTime = -1.f;
	
	SetTickableTickType(ETickableTickType::Conditional);
	
	TryUpdateLoadingScreen();
}

void USL_LoadingScreenSubsystem::OnMapPostLoaded(UWorld* LoadedWorld)
{
	if (LoadedWorld && LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;
		HoldGameplayForLoadingScreen();
		TryUpdateLoadingScreen();
	}
}

void USL_LoadingScreenSubsystem::OnWorldBeginTearDown(UWorld* World)
{
	if (!World || World->GetGameInstance() != GetGameInstance()) return;

	// Map travel tears down the old world after the next map has already asked for the loading screen.
	if (bIsCurrentlyLoadingMap || bForceShowLoadingScreen) return;

	bIsShuttingDown = true;
	SetTickableTickType(ETickableTickType::Never);
	TryRemoveLoadingScreen();
}

void USL_LoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	// Check if there is any startup loading screen that is currently active
	if (IsPreLoadScreenActive()) return;

	const USL_LoadingScreenSettings* LoadingScreenSettings = GetDefault<USL_LoadingScreenSettings>();
	const bool bShowLoadingScreen = !GIsEditor || (LoadingScreenSettings && LoadingScreenSettings->bShowLoadingScreenInEditor);
	
	if (!bShowLoadingScreen)
	{
		if (bIsShuttingDown || CheckTheNeedToShowLoadingScreen()) return;

		TryRemoveLoadingScreen();
		SetTickableTickType(ETickableTickType::Never);
		
		return;
	}
	
	// Check if we should show the loading screen
	if (ShouldShowLoadingScreen())
	{
		// Try display the loading screen here
		TryDisplayLoadingScreenIfNone();
		
		OnLoadingReasonUpdated.Broadcast(CurrentLoadingReason);
	}
	else
	{
		// Try to remove the current active loading screen
		TryRemoveLoadingScreen();
		
		HoldLoadingScreenStartupTime = -1.f;
		
		// Disable ticking
		SetTickableTickType(ETickableTickType::Never);
	}
}

bool USL_LoadingScreenSubsystem::IsPreLoadScreenActive() const
{
	if (FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}
	
	return false;
}

bool USL_LoadingScreenSubsystem::ShouldShowLoadingScreen()
{
	if (bIsShuttingDown) return false;

	const USL_LoadingScreenSettings* LoadingScreenSettings = GetDefault<USL_LoadingScreenSettings>();

	UGameInstance* GameInstance = GetGameInstance();
	UGameViewportClient* ViewportClient = GameInstance ? GameInstance->GetGameViewportClient() : nullptr;
	if (!ViewportClient) return false;

	UWorld* World = GameInstance->GetWorld();
	const bool bCanRenderWorld = CanRenderWorldUnderLoadingScreen(World);
	
	if (CheckTheNeedToShowLoadingScreen())
	{
		HoldLoadingScreenStartupTime = -1.f;
		ViewportClient->bDisableWorldRendering = !bCanRenderWorld;
		HoldGameplayForLoadingScreen();
		
		return true;
	}

	ViewportClient->bDisableWorldRendering = false;
	HoldGameplayForLoadingScreen();

	if (CheckTheNeedToSettleWorld(World))
	{
		HoldLoadingScreenStartupTime = -1.f;
		
		return true;
	}
	
	CurrentLoadingReason = TEXT("Waiting for Texture Streaming");
	
	const float CurrentTime = FPlatformTime::Seconds();
	
	if (HoldLoadingScreenStartupTime < 0.f)
	{
		HoldLoadingScreenStartupTime = CurrentTime;
	}
	
	const float ExtraHoldSeconds = LoadingScreenSettings ? LoadingScreenSettings->HoldLoadingScreenExtraSeconds : 0.f;
	const float ElapsedTime = CurrentTime - HoldLoadingScreenStartupTime;
	
	if (ElapsedTime < ExtraHoldSeconds) return true;
	
	return false;
}

bool USL_LoadingScreenSubsystem::CheckTheNeedToShowLoadingScreen()
{
	if (bForceShowLoadingScreen)
	{
		CurrentLoadingReason = TEXT("Loading");
		
		return true;
	}

	if (bIsCurrentlyLoadingMap)
	{
		CurrentLoadingReason = TEXT("Loading level");
		
		return true;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		CurrentLoadingReason = TEXT("Initializing game");
		
		return true;
	}

	const FWorldContext* WorldContext = GameInstance->GetWorldContext();
	if (!WorldContext)
	{
		CurrentLoadingReason = TEXT("Initializing world");
		
		return true;
	}

	if (!WorldContext->TravelURL.IsEmpty())
	{
		CurrentLoadingReason = TEXT("Travel pending");
		
		return true;
	}

	if (WorldContext->PendingNetGame)
	{
		CurrentLoadingReason = TEXT("Connecting");
		
		return true;
	}
	
	UWorld* OwningWorld = WorldContext->World();
	if (!OwningWorld)
	{
		CurrentLoadingReason = TEXT("Initializing world");
		
		return true;
	}

	if (OwningWorld->bIsTearingDown) return false;

	if (IsAsyncLoading())
	{
		CurrentLoadingReason = TEXT("Loading assets");
		
		return true;
	}
	
	if (!OwningWorld->HasBegunPlay())
	{
		CurrentLoadingReason = TEXT("World has not begun play yet");
		
		return true;
	}

	if (OwningWorld->IsInSeamlessTravel())
	{
		CurrentLoadingReason = TEXT("Seamless travel");
		
		return true;
	}

	if (!OwningWorld->GetGameState())
	{
		CurrentLoadingReason = TEXT("Waiting for game state");
		
		return true;
	}
	
	if (!OwningWorld->GetFirstPlayerController())
	{
		CurrentLoadingReason = TEXT("Player controller is not valid yet");
		
		return true;
	}

	if (IsGameplayWorld(OwningWorld) && !OwningWorld->GetFirstPlayerController()->GetPawn())
	{
		CurrentLoadingReason = TEXT("Waiting for player pawn");
		
		return true;
	}

	if (OwningWorld->HasStreamingLevelsToConsider())
	{
		CurrentLoadingReason = TEXT("Streaming levels");
		
		return true;
	}
	
	return false;
}

bool USL_LoadingScreenSubsystem::CheckTheNeedToSettleWorld(UWorld* World)
{
	if (!CanRenderWorldUnderLoadingScreen(World)) return false;

	++WorldSettleFrames;
	if (WorldSettleFrames < SLLoadingScreen::MinWorldSettleFrames)
	{
		CurrentLoadingReason = TEXT("Warming up materials");
		return true;
	}

	const bool bHasShaderJobs = GShaderCompilingManager && GShaderCompilingManager->GetNumRemainingJobs() > 0;
	const bool bHasPSOJobs = FShaderPipelineCache::NumPrecompilesRemaining() > 0;
	if (bHasShaderJobs || bHasPSOJobs)
	{
		const float CurrentTime = FPlatformTime::Seconds();
		if (ShaderWaitStartTime < 0.f)
		{
			ShaderWaitStartTime = CurrentTime;
		}

		if ((CurrentTime - ShaderWaitStartTime) < SLLoadingScreen::MaxShaderWaitSeconds)
		{
			CurrentLoadingReason = TEXT("Compiling shaders");
			return true;
		}
	}
	
	return false;
}

void USL_LoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	if (bIsShuttingDown) return;

	UGameInstance* GameInstance = GetGameInstance();
	UGameViewportClient* ViewportClient = GameInstance ? GameInstance->GetGameViewportClient() : nullptr;
	if (!GameInstance || !ViewportClient) return;

	if (!CachedCreatedLoadingScreenWidget.IsValid())
	{
		const USL_LoadingScreenSettings* LoadingScreenSettings = GetDefault<USL_LoadingScreenSettings>();
		if (!LoadingScreenSettings || LoadingScreenSettings->SoftLoadingScreenWidgetClass.IsNull()) return;

		const TSubclassOf<UUserWidget> LoadedWidgetClass = LoadingScreenSettings->SoftLoadingScreenWidgetClass.LoadSynchronous();
		if (!LoadedWidgetClass) return;

		UUserWidget* CreatedWidget = UUserWidget::CreateWidgetInstance(*GameInstance, LoadedWidgetClass, NAME_None);
		if (!CreatedWidget) return;

		CachedLoadingScreenUserWidget = CreatedWidget;
		CachedCreatedLoadingScreenWidget = CreatedWidget->TakeWidget();
	}

	if (!bLoadingScreenVisible)
	{
		ViewportClient->AddViewportWidgetContent(
			CachedCreatedLoadingScreenWidget.ToSharedRef(),
			1000
		);

		bLoadingScreenVisible = true;
		ApplyLoadingScreenPerformanceSettings(true);
		NotifyLoadingScreenVisibilityChanged(true);
		ForceLoadingScreenPresent();
	}

	HoldGameplayForLoadingScreen();
}

void USL_LoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	UGameInstance* GameInstance = GetGameInstance();
	UGameViewportClient* ViewportClient = GameInstance ? GameInstance->GetGameViewportClient() : nullptr;

	if (ViewportClient)
	{
		ViewportClient->bDisableWorldRendering = false;

		if (bLoadingScreenVisible && CachedCreatedLoadingScreenWidget.IsValid())
		{
			ViewportClient->RemoveViewportWidgetContent(CachedCreatedLoadingScreenWidget.ToSharedRef());
		}
	}

	const bool bWasVisible = bLoadingScreenVisible;
	bLoadingScreenVisible = false;
	bForceShowLoadingScreen = false;

	if (bWasVisible)
	{
		ApplyLoadingScreenPerformanceSettings(false);
	}

	NotifyLoadingScreenVisibilityChanged(false);
	ReleaseGameplayHold();
}

void USL_LoadingScreenSubsystem::NotifyLoadingScreenVisibilityChanged(bool bIsVisible)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;

	for (ULocalPlayer* ExistingLocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (!ExistingLocalPlayer) continue;
		
		if (APlayerController* PC = ExistingLocalPlayer->GetPlayerController(GameInstance->GetWorld()))
		{
			// Query if the player controller implements the interface. Call the function through the interface to notify the loading status if yes
			if (PC->Implements<USL_LoadingScreenInterface>())
			{
				if (bIsVisible)
				{
					ISL_LoadingScreenInterface::Execute_OnLoadingScreenActivated(PC);
				}
				else
				{
					ISL_LoadingScreenInterface::Execute_OnLoadingScreenDeactivated(PC);
				}
			}
			
			if (APawn* OwningPawn = PC->GetPawn())
			{
				if (OwningPawn->Implements<USL_LoadingScreenInterface>())
				{
					if (bIsVisible)
					{
						ISL_LoadingScreenInterface::Execute_OnLoadingScreenActivated(OwningPawn);
					}
					else
					{
						ISL_LoadingScreenInterface::Execute_OnLoadingScreenDeactivated(OwningPawn);
					}
				}
			}
		}
	}
}

void USL_LoadingScreenSubsystem::ForceLoadingScreenPresent()
{
	if (!FSlateApplication::IsInitialized()) return;

	FSlateApplication::Get().Tick();
}

void USL_LoadingScreenSubsystem::ApplyLoadingScreenPerformanceSettings(bool bEnableLoadingScreen)
{
	UGameInstance* GameInstance = GetGameInstance();
	UGameViewportClient* ViewportClient = GameInstance ? GameInstance->GetGameViewportClient() : nullptr;
	if (!ViewportClient) return;

	FShaderPipelineCache::SetBatchMode(
		bEnableLoadingScreen ? FShaderPipelineCache::BatchMode::Fast : FShaderPipelineCache::BatchMode::Background);

	if (UWorld* ViewportWorld = ViewportClient->GetWorld())
	{
		if (AWorldSettings* WorldSettings = ViewportWorld->GetWorldSettings(false, false))
		{
			WorldSettings->bHighPriorityLoadingLocal = bEnableLoadingScreen;
		}
	}
}

void USL_LoadingScreenSubsystem::HoldGameplayForLoadingScreen()
{
	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	
	if (!IsGameplayWorld(World)) return;

	bHoldGameplay = true;
	bGameplayReady = false;
}

void USL_LoadingScreenSubsystem::ReleaseGameplayHold()
{
	const bool bShouldNotifyReady = bHoldGameplay || !bGameplayReady;
	
	bHoldGameplay = false;
	bGameplayReady = true;

	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	
	if (World && !World->bIsTearingDown && IsGameplayWorld(World))
	{
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->TimeDilation = 1.f;
			WorldSettings->CinematicTimeDilation = 1.f;
			WorldSettings->DemoPlayTimeDilation = 1.f;
			WorldSettings->SetTimeDilation(1.f);
		}
	}

	if (bShouldNotifyReady)
	{
		OnGameplayReady.Broadcast();
	}
}

bool USL_LoadingScreenSubsystem::IsGameplayWorld(const UWorld* World) const
{
	return World && !World->GetAuthGameMode<ASL_MainMenuGameMode>();
}

bool USL_LoadingScreenSubsystem::CanRenderWorldUnderLoadingScreen(const UWorld* World) const
{
	return World && !World->bIsTearingDown && World->HasBegunPlay() && !bIsCurrentlyLoadingMap;
}