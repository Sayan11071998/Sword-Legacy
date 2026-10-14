#include "Subsystems/SL_LoadingScreenSubsystem.h"
#include "PreLoadScreenManager.h"
#include "Utilities/SL_LoadingScreenSettings.h"

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
}

void USL_LoadingScreenSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
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
	
	return GetGameInstance() && GetGameInstance()->GetGameViewportClient();
}

TStatId USL_LoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USL_LoadingScreenSubsystem, STATGROUP_Tickables);
}

void USL_LoadingScreenSubsystem::OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance()) return;
	
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

void USL_LoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	// Check if there is any startup loading screen that is currently active
	if (IsPreLoadScreenActive()) return;
	
	// Check if we should show the loading screen
	if (ShouldShowLoadingScreen())
	{
		// Try display the loading screen here
		
		OnLoadingReasonUpdated.Broadcast(CurrentLoadingReason);
	}
	else
	{
		// Try to remove the current active loading screen
		
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
	const USL_LoadingScreenSettings* LoadingScreenSettings = GetDefault<USL_LoadingScreenSettings>();
	
	if (GIsEditor && !LoadingScreenSettings->bShowLoadingScreenInEditor)
	{
		return false;
	}
	
	// Check if the objects in the world need a loading screen
	if (CheckTheNeedToShowLoadingScreen())
	{
		GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = true;
		
		return true;
	}
	
	CurrentLoadingReason = TEXT("Waiting for Texture Streaming");
	
	// There is no need to show the loading screen. Allow the world to be renderer to our viewport here
	GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = false;
	
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