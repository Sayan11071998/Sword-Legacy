#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_AsyncAction_PushConfirmScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmScreenButtonClickedDelegate, ESL_ConfirmScreenButtonType, ClickedButtonType);

UCLASS()
class SWORD_LEGACY_API USL_AsyncAction_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	// ~ Begin UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
	// ~ End UBlueprintAsyncActionBase Interface
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Show Confirmation Screen"))
	static USL_AsyncAction_PushConfirmScreen* PushConfirmScreen(
		const UObject* WorldContextObject,
		ESL_ConfirmScreenType ScreenType,
		FText InScreenTitle,
		FText InScreenMessage
	);
	
	UPROPERTY(BlueprintAssignable)
	FOnConfirmScreenButtonClickedDelegate OnButtonClicked;
	
private:
	UPROPERTY()
	TWeakObjectPtr<UWorld> CachedOwningWorld;
	
	UPROPERTY()
	ESL_ConfirmScreenType CachedScreenType;
	
	UPROPERTY()
	FText CachedScreenTitle;
	
	UPROPERTY()
	FText CachedScreenMessage;
};