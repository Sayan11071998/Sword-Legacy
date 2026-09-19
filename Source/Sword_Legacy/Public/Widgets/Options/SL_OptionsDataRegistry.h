#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SL_OptionsDataRegistry.generated.h"

class USL_ListDataObject_Base;
class USL_ListDataObject_Collection;

UCLASS()
class SWORD_LEGACY_API USL_OptionsDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	// Gets called by options screen right after the object of type USL_OptionsDataRegistry is created.
	void InitOptionsDataRegistry(TObjectPtr<ULocalPlayer> InOwningLocalPlayer);
	
	const TArray<TObjectPtr<USL_ListDataObject_Collection>>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections; }
	
	const TArray<TObjectPtr<USL_ListDataObject_Base>> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID);
	
private:
	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab();
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<USL_ListDataObject_Collection>> RegisteredOptionsTabCollections;
};