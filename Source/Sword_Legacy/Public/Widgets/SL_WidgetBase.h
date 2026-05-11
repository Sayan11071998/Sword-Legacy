#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SL_WidgetBase.generated.h"

class USL_PlayerUIComponent;

UCLASS()
class SWORD_LEGACY_API USL_WidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitEnemyCreatedWidget(AActor* OwningEnemyActor);
	
protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~ End UUserWidget Interface
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Owning Player UI Component Initialized"))
	void BP_OnOwningPlayerUIComponentInitialized(USL_PlayerUIComponent* OwningPlayerUIComponent);
};