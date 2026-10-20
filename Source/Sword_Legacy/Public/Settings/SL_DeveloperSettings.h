#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "SL_DeveloperSettings.generated.h"

class UDataTable;
class UWorld;
class USL_Widget_Activatable_Base;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game UI Settings"))
class SWORD_LEGACY_API USL_DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta = (ForceInLineRow, Categories = "UI.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<USL_Widget_Activatable_Base>> GameWidgetMap;
	
	UPROPERTY(Config, EditAnywhere, Category = "Widget Image Reference", meta = (ForceInLineRow, Categories = "UI.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;
	
	UPROPERTY(Config, EditAnywhere, Category = "Credits", meta = (RequiredAssetDataTags = "RowStructure=/Script/Sword_Legacy.SL_CreditTableRow"))
	TSoftObjectPtr<UDataTable> CreditsTable;

	UPROPERTY(Config, EditAnywhere, Category = "Controls", meta = (RequiredAssetDataTags = "RowStructure=/Script/Sword_Legacy.SL_ControlOrderTableRow"))
	TSoftObjectPtr<UDataTable> ControlOrderTable;

	UPROPERTY(Config, EditAnywhere, Category = "Level Reference", meta = (ForceInLineRow, Categories = "Game.Level"))
	TMap<FGameplayTag, TSoftObjectPtr<UWorld>> GameLevelMap;
};