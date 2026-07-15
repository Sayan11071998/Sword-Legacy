#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SL_UISubsystem.generated.h"

class USL_Widget_PrimaryLayout;

UCLASS()
class SWORD_LEGACY_API USL_UISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static TObjectPtr<USL_UISubsystem> Get(const TObjectPtr<UObject> WorldContextObject);
	
	// ~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// ~ End USubsystem Interface
	
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(USL_Widget_PrimaryLayout* InCreatedWidget);
	
private:
	UPROPERTY(Transient)
	TObjectPtr<USL_Widget_PrimaryLayout> CreatedPrimaryLayout;
};