#pragma once

#include "CoreMinimal.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SL_UISubsystem.generated.h"

class USL_Widget_Activatable_Base;
struct FGameplayTag;
class USL_Widget_PrimaryLayout;
class USL_CommonButtonBase;

enum class EAsyncPushWidgetState : uint8
{
	OnCreatedBeforePush,
	AfterPush
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, USL_CommonButtonBase*, BroadcastingButton, FText, DescriptionText);

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
	
	void PushConfirmScreenToModalStackAsync(
		ESL_ConfirmScreenType InScreenType,
		const FText& InScreenTitle,
		const FText& InScreenMsg,
		TFunction<void(ESL_ConfirmScreenButtonType)> ButtonClickedCallback
	);
	
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(USL_Widget_PrimaryLayout* InCreatedWidget);
	
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<USL_Widget_PrimaryLayout> CreatedPrimaryLayout;
};