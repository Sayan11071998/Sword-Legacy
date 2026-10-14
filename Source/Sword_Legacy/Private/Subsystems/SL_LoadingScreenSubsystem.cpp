#include "Subsystems/SL_LoadingScreenSubsystem.h"
#include "PreLoadScreenManager.h"
#include "Utilities/SL_LoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"

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
}

void USL_LoadingScreenSubsystem::Deinitialize()
{
	bIsShuttingDown = true;
	SetTickableTickType(ETickableTickType::Never);
	TryRemoveLoadingScreen();

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

TStatId USL_LoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USL_LoadingScreenSubsystem, STATGROUP_Tickables);
}

void USL_LoadingScreenSubsystem::OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance()) return;

	bIsShuttingDown = false;
	SetTickableTickType(ETickableTickType::Conditional);
	
	bIsCurrentlyLoadingMap = true;
	
	TryUpdateLoadingScreen();
}

void USL_LoadingScreenSubsystem::OnMapPostLoaded(UWorld* LoadedWorld)
{
	if (LoadedWorld && LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;
	}
}

void USL_LoadingScreenSubsystem::OnWorldBeginTearDown(UWorld* World)
{
	if (!World || World->GetGameInstance() != GetGameInstance()) return;

	// Map travel tears down the old world after the next map has already asked for the loading screen.
	if (bIsCurrentlyLoadingMap) return;

	bIsShuttingDown = true;
	SetTickableTickType(ETickableTickType::Never);
	TryRemoveLoadingScreen();
}

void USL_LoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	// Check if there is any startup loading screen that is currently active
	if (IsPreLoadScreenActive()) return;
	
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
		
		// Notify the loading is complete
		
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
	
	if (GIsEditor && !LoadingScreenSettings->bShowLoadingScreenInEditor)
	{
		return false;
	}

	UGameViewportClient* ViewportClient = GetGameInstance() ? GetGameInstance()->GetGameViewportClient() : nullptr;
	if (!ViewportClient) return false;
	
	// Check if the objects in the world need a loading screen
	if (CheckTheNeedToShowLoadingScreen())
	{
		ViewportClient->bDisableWorldRendering = true;
		
		return true;
	}
	
	CurrentLoadingReason = TEXT("Waiting for Texture Streaming");
	
	// There is no need to show the loading screen. Allow the world to be renderer to our viewport here
	ViewportClient->bDisableWorldRendering = false;
	
	const float CurrentTime = FPlatformTime::Seconds();
	
	if (HoldLoadingScreenStartupTime < 0.f)
	{
		HoldLoadingScreenStartupTime = CurrentTime;
	}
	
	const float ElapsedTime = CurrentTime - HoldLoadingScreenStartupTime;
	
	if (ElapsedTime < LoadingScreenSettings->HoldLoadingScreenExtraSeconds)
	{
		return true;
	}
	
	return false;
}

bool USL_LoadingScreenSubsystem::CheckTheNeedToShowLoadingScreen()
{
	if (bIsCurrentlyLoadingMap)
	{
		CurrentLoadingReason = TEXT("Loading level");
		
		return true;
	}
	
	UWorld* OwningWorld = GetGameInstance()->GetWorld();
	if (!OwningWorld)
	{
		CurrentLoadingReason = TEXT("Initializing world");
		
		return true;
	}

	if (OwningWorld->bIsTearingDown) return false;
	
	if (!OwningWorld->HasBegunPlay())
	{
		CurrentLoadingReason = TEXT("World has not begun play yet");
		
		return true;
	}
	
	if (!OwningWorld->GetFirstPlayerController())
	{
		CurrentLoadingReason = TEXT("Player controller is not valid yet");
		
		return true;
	}
	
	// Check if the game state, player state, or player character, actor components are ready
	
	return false;
}

void USL_LoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	if (bIsShuttingDown || CachedCreatedLoadingScreenWidget.IsValid()) return;

	UGameInstance* GameInstance = GetGameInstance();
	UGameViewportClient* ViewportClient = GameInstance ? GameInstance->GetGameViewportClient() : nullptr;
	if (!GameInstance || !ViewportClient) return;
	
	const USL_LoadingScreenSettings* LoadingScreenSettings = GetDefault<USL_LoadingScreenSettings>();
	
	TSubclassOf<UUserWidget> LoadedWidgetClass = LoadingScreenSettings->GetLoadingScreenWidgetClassChecked();
	
	UUserWidget* CreatedWidget = UUserWidget::CreateWidgetInstance(*GameInstance, LoadedWidgetClass, NAME_None);
	
	check(CreatedWidget);

	CachedLoadingScreenUserWidget = CreatedWidget;
	CachedCreatedLoadingScreenWidget = CreatedWidget->TakeWidget();
	
	ViewportClient->AddViewportWidgetContent(
		CachedCreatedLoadingScreenWidget.ToSharedRef(),
		1000
	);
}

void USL_LoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	UGameInstance* GameInstance = GetGameInstance();
	UGameViewportClient* ViewportClient = GameInstance ? GameInstance->GetGameViewportClient() : nullptr;

	if (ViewportClient)
	{
		ViewportClient->bDisableWorldRendering = false;

		if (CachedCreatedLoadingScreenWidget.IsValid())
		{
			ViewportClient->RemoveViewportWidgetContent(CachedCreatedLoadingScreenWidget.ToSharedRef());
		}
	}

	CachedCreatedLoadingScreenWidget.Reset();
	CachedLoadingScreenUserWidget = nullptr;
}