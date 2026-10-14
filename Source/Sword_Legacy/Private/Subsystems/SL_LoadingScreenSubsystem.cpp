#include "Subsystems/SL_LoadingScreenSubsystem.h"

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
	
	// Check if we should show the loading screen
	if (true)
	{
		// Try display the loading screen here
	}
	else
	{
		// Try to remove the current active loading screen
		
		// Notify the loading is complete
		
		// Disable ticking
		SetTickableTickType(ETickableTickType::Never);
	}
}