#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SL_UISubsystem.generated.h"

class USL_Widget_Activatable_Base;
struct FGameplayTag;
class USL_Widget_PrimaryLayout;

enum class EAsyncPushWidgetState : uint8
{
	OnCreatedBeforePush,
	AfterPush
};

UCLASS()
class SWORD_LEGACY_API USL_UISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static TObjectPtr<USL_UISubsystem> Get(const TObjectPtr<UObject> WorldContextObject);
	
	// ~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// ~ End USubsystem Interface
	
	void PushSoftWidgetToStackAsync(
		const FGameplayTag& InWidgetStackTag,
		TSoftClassPtr<USL_Widget_Activatable_Base> InSoftWidgetClass,
		TFunction<void(EAsyncPushWidgetState, TObjectPtr<USL_Widget_Activatable_Base>)> AsyncPushStateCallback
	);
	
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(USL_Widget_PrimaryLayout* InCreatedWidget);
	
private:
	UPROPERTY(Transient)
	TObjectPtr<USL_Widget_PrimaryLayout> CreatedPrimaryLayout;
};