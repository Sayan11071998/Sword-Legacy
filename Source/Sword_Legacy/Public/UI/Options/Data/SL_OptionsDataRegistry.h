#pragma once

#include "CoreMinimal.h"
#include "SL_OptionsDataRegistry.generated.h"

class USL_ListDataObject_Base;
class USL_ListDataObject_Collection;
class UEnhancedInputUserSettings;

UCLASS()
class SWORD_LEGACY_API USL_OptionsDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	// Gets called by options screen right after the object of type USL_OptionsDataRegistry is created.
	void InitOptionsDataRegistry(TObjectPtr<ULocalPlayer> InOwningLocalPlayer);
	
	const TArray<TObjectPtr<USL_ListDataObject_Collection>>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections; }
	
	const TArray<USL_ListDataObject_Base*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID);
	
	void RegisterPlayerMappableMappingContexts(UEnhancedInputUserSettings* EIUserSettings) const;
	
private:
	void FindChildListDataRecursively(
		USL_ListDataObject_Base* InParentData,
		TArray<USL_ListDataObject_Base*>& OutFoundChildListData
	) const;
	
	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab(TObjectPtr<ULocalPlayer> InOwningLocalPlayer);
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<USL_ListDataObject_Collection>> RegisteredOptionsTabCollections;
};