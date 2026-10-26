#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SL_LoadingScreenSubsystem.generated.h"

class UUserWidget;

UCLASS()
class SWORD_LEGACY_API USL_LoadingScreenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingReasonUpdatedDelegate, const FString&, CurrentLoadingReason);
	DECLARE_MULTICAST_DELEGATE(FOnGameplayReadyAfterLoading);
	
	// ~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~ End USubsystem Interface
	
	// ~ Begin FTickableGameObject Interface
	virtual UWorld* GetTickableGameObjectWorld() const override;
	// ~ End FTickableGameObject Interface
	
	// ~ Begin FTickableObjectBase Interface
	virtual void Tick( float DeltaTime ) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	// ~ End FTickableObjectBase Interface

	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
	void ShowLoadingScreen(const FString& Reason = TEXT("Loading"));

	UFUNCTION(BlueprintPure, Category = "Loading Screen")
	bool IsHoldingGameplay() const { return bHoldGameplay; }

	UFUNCTION(BlueprintPure, Category = "Loading Screen")
	bool IsGameplayReady() const { return bGameplayReady; }

	FOnGameplayReadyAfterLoading OnGameplayReady;
	
	UPROPERTY(BlueprintAssignable)
	FOnLoadingReasonUpdatedDelegate OnLoadingReasonUpdated;
	
private:
	void OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName);
	void OnMapPostLoaded(UWorld* LoadedWorld);
	void OnWorldBeginTearDown(UWorld* World);
	
	void TryUpdateLoadingScreen();
	
	bool IsPreLoadScreenActive() const;
	
	bool ShouldShowLoadingScreen();
	
	bool CheckTheNeedToShowLoadingScreen();
	bool CheckTheNeedToSettleWorld(UWorld* World);
	
	void TryDisplayLoadingScreenIfNone();
	void TryRemoveLoadingScreen();
	
	void NotifyLoadingScreenVisibilityChanged(bool bIsVisible);

	void ForceLoadingScreenPresent();
	
	void ApplyLoadingScreenPerformanceSettings(bool bEnableLoadingScreen);
	
	void HoldGameplayForLoadingScreen();
	
	void ReleaseGameplayHold();
	
	bool IsGameplayWorld(const UWorld* World) const;
	
	bool CanRenderWorldUnderLoadingScreen(const UWorld* World) const;
	
	bool bIsCurrentlyLoadingMap = false;
	bool bForceShowLoadingScreen = false;
	bool bLoadingScreenVisible = false;
	bool bHoldGameplay = false;
	bool bGameplayReady = true;
	bool bIsShuttingDown = false;
	
	int32 WorldSettleFrames = 0;
	
	float ShaderWaitStartTime = -1.f;
	float HoldLoadingScreenStartupTime = -1.f;
	
	FString CurrentLoadingReason;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> CachedLoadingScreenUserWidget;

	TSharedPtr<SWidget> CachedCreatedLoadingScreenWidget;
};