#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SL_LoadingScreenSubsystem.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_LoadingScreenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
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
	virtual TStatId GetStatId() const override;
	// ~ End FTickableObjectBase Interface
	
private:
	void OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName);
	void OnMapPostLoaded(UWorld* LoadedWorld);
};