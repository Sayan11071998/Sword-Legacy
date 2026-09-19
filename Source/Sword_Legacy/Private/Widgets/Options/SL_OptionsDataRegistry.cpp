#include "Widgets/Options/SL_OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"

void USL_OptionsDataRegistry::InitOptionsDataRegistry(TObjectPtr<ULocalPlayer> InOwningLocalPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab();
}

const TArray<TObjectPtr<USL_ListDataObject_Base>> USL_OptionsDataRegistry::GetListSourceItemsBySelectedTabID(
	const FName& InSelectedTabID)
{
	USL_ListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](USL_ListDataObject_Collection* AvailableTabCollection)->bool
		{
			return AvailableTabCollection->GetDataID() == InSelectedTabID;
		}
	);
	
	checkf(FoundTabCollectionPtr, TEXT("No valid tab found under the ID: %S"), *InSelectedTabID.ToString());
	
	USL_ListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;
	
	return FoundTabCollection->GetAllChildListData();
}

void USL_OptionsDataRegistry::InitGameplayCollectionTab()
{
	USL_ListDataObject_Collection* GameplayTabCollection = NewObject<USL_ListDataObject_Collection>();
	GameplayTabCollection->SetDataID(FName(TEXT("GameplayTabCollection")));
	GameplayTabCollection->SetDataDisplayName(FText::FromString(TEXT("Gameplay")));
	
	// Game Difficulty
	{
		USL_ListDataObject_String* GameDifficulty = NewObject<USL_ListDataObject_String>();
		GameDifficulty->SetDataID(FName(TEXT("GameDifficulty")));
		GameDifficulty->SetDataDisplayName(FText::FromString(TEXT("Difficulty")));
		
		GameplayTabCollection->AddChildListData(GameDifficulty);
	}
	
	// Test Item
	{
		USL_ListDataObject_String* TestItem = NewObject<USL_ListDataObject_String>();
		TestItem->SetDataID(FName(TEXT("TestItem")));
		TestItem->SetDataDisplayName(FText::FromString(TEXT("Test Item")));
		
		GameplayTabCollection->AddChildListData(TestItem);
	}
	
	RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}

void USL_OptionsDataRegistry::InitAudioCollectionTab()
{
	USL_ListDataObject_Collection* AudioTabCollection = NewObject<USL_ListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName(TEXT("AudioTabCollection")));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("Audio")));
	
	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void USL_OptionsDataRegistry::InitVideoCollectionTab()
{
	USL_ListDataObject_Collection* VideoTabCollection = NewObject<USL_ListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName(TEXT("VideoTabCollection")));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("Video")));
	
	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void USL_OptionsDataRegistry::InitControlCollectionTab()
{
	USL_ListDataObject_Collection* ControlTabCollection = NewObject<USL_ListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName(TEXT("ControlTabCollection")));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("Control")));
	
	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}