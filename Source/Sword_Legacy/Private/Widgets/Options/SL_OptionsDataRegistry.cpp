#include "Widgets/Options/SL_OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "Utilities/SL_OptionsDataInteractionHelper.h"
#include "Utilities/SL_GameUserSettings.h"

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
	MakeShared<FSL_OptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(USL_GameUserSettings, SetterOrGetterFuncName))

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
	const TObjectPtr<USL_ListDataObject_Collection>* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](const TObjectPtr<USL_ListDataObject_Collection>& AvailableTabCollection)->bool
		{
			return AvailableTabCollection && AvailableTabCollection->GetDataID() == InSelectedTabID;
		}
	);
	
	checkf(FoundTabCollectionPtr, TEXT("No valid tab found under the ID: %s"), *InSelectedTabID.ToString());
	
	USL_ListDataObject_Collection* FoundTabCollection = FoundTabCollectionPtr->Get();
	
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
		GameDifficulty->AddDynamicOption(TEXT("Easy"), FText::FromString(TEXT("Easy")));
		GameDifficulty->AddDynamicOption(TEXT("Normal"), FText::FromString(TEXT("Normal")));
		GameDifficulty->AddDynamicOption(TEXT("Hard"), FText::FromString(TEXT("Hard")));
		GameDifficulty->AddDynamicOption(TEXT("Very Hard"), FText::FromString(TEXT("Very Hard")));
		GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty));
		GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentGameDifficulty));
		GameDifficulty->SetShouldApplySettingsImmediately(true);
		
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