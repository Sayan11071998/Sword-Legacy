#pragma once

#include "CoreMinimal.h"
#include "Components/UI/SL_PawnUIComponent.h"
#include "SL_EnemyUIComponent.generated.h"

class USL_WidgetBase;

UCLASS()
class SWORD_LEGACY_API USL_EnemyUIComponent : public USL_PawnUIComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDrawnWidget(USL_WidgetBase* InWidgetToRegister);
	
	UFUNCTION(BlueprintCallable)
	void RemoveEnemyDrawnWidgetsIfAny();
	
private:
	UPROPERTY()
	TArray<TObjectPtr<USL_WidgetBase>> EnemyDrawnWidgets;
};