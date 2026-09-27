#include "Widgets/Options/SL_OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "Utilities/SL_OptionsDataInteractionHelper.h"
#include "Utilities/SL_GameUserSettings.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_StringBool.h"

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
	MakeShared<FSL_OptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(USL_GameUserSettings, SetterOrGetterFuncName))

void USL_OptionsDataRegistry::InitOptionsDataRegistry(TObjectPtr<ULocalPlayer> InOwningLocalPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab();
}

const TArray<USL_ListDataObject_Base*> USL_OptionsDataRegistry::GetListSourceItemsBySelectedTabID(
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
	
	TArray<USL_ListDataObject_Base*> AllChildListItems;
	
	for (USL_ListDataObject_Base* ChildListData : FoundTabCollection->GetAllChildListData())
	{
		if (!ChildListData) continue;
		
		AllChildListItems.Add(ChildListData);
		
		if (ChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(ChildListData, AllChildListItems);
		}
	}
	
	return AllChildListItems;
}

void USL_OptionsDataRegistry::FindChildListDataRecursively(USL_ListDataObject_Base* InParentData,
	TArray<USL_ListDataObject_Base*>& OutFoundChildListData) const
{
	if (!InParentData || !InParentData->HasAnyChildListData()) return;
	
	for (USL_ListDataObject_Base* SubChildListData : InParentData->GetAllChildListData())
	{
		if (!SubChildListData) continue;
		
		OutFoundChildListData.Add(SubChildListData);
		
		if (SubChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(SubChildListData, OutFoundChildListData);
		}
	}
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
		GameDifficulty->SetDescriptionRichText(FText::FromString(TEXT("Adjusts the difficulty of the game experience.\n\n<Bold>Easy:</> Focuses on the story experience. "
			"Provides the most relaxing combat.\n\n<Bold>Normal:</> Offers slightly harder combat experience\n\n<Bold>Hard:</>"
			"Offers a much more challenging combat experience\n\n<Bold>Vert Hard:</> Provides the most challenging combat experience. Not recommended for first play through.")));
		GameDifficulty->AddDynamicOption(TEXT("Easy"), FText::FromString(TEXT("Easy")));
		GameDifficulty->AddDynamicOption(TEXT("Normal"), FText::FromString(TEXT("Normal")));
		GameDifficulty->AddDynamicOption(TEXT("Hard"), FText::FromString(TEXT("Hard")));
		GameDifficulty->AddDynamicOption(TEXT("Very Hard"), FText::FromString(TEXT("Very Hard")));
		GameDifficulty->SetDefaultValueFromString(TEXT("Normal"));
		GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty));
		GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentGameDifficulty));
		GameDifficulty->SetShouldApplySettingsImmediately(true);
		
		GameplayTabCollection->AddChildListData(GameDifficulty);
	}
	
	// Test Item
	{
		USL_ListDataObject_String* TestItem = NewObject<USL_ListDataObject_String>();
		TestItem->SetDataID(FName(TEXT("TestItem")));
		TestItem->SetDataDisplayName(FText::FromString(TEXT("Test Image Item")));
		TestItem->SetSoftDescriptionImage(USL_FunctionLibrary::GetOptionsSoftImageByTag(SL_GameplayTags::UI_Image_TestImage));
		TestItem->SetDescriptionRichText(FText::FromString(TEXT("The image to display can be specified in the project settings.")));
		
		GameplayTabCollection->AddChildListData(TestItem);
	}
	
	RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}

void USL_OptionsDataRegistry::InitAudioCollectionTab()
{
	USL_ListDataObject_Collection* AudioTabCollection = NewObject<USL_ListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName(TEXT("AudioTabCollection")));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("Audio")));
	
	// Volume Category
	{
		USL_ListDataObject_Collection* VolumeCategoryCollection = NewObject<USL_ListDataObject_Collection>();
		VolumeCategoryCollection->SetDataID(FName(TEXT("VolumeCategoryCollection")));
		VolumeCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Volume")));
		
		AudioTabCollection->AddChildListData(VolumeCategoryCollection);
		
		// Overall Volume
		{
			USL_ListDataObject_Scalar* OverallVolume = NewObject<USL_ListDataObject_Scalar>();
			OverallVolume->SetDataID(FName(TEXT("OverallVolume")));
			OverallVolume->SetDataDisplayName(FText::FromString(TEXT("Overall Volume")));
			OverallVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for overall volume")));
			OverallVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			OverallVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			OverallVolume->SetSliderStepSize(0.01f);
			OverallVolume->SetDefaultValueFromString(LexToString(1.f));
			OverallVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			OverallVolume->SetNumberFormattingOptions(USL_ListDataObject_Scalar::NoDecimal());
			OverallVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
			OverallVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));
			OverallVolume->SetShouldApplySettingsImmediately(true);
			
			VolumeCategoryCollection->AddChildListData(OverallVolume);
		}
		
		// Music Volume
		{
			USL_ListDataObject_Scalar* MusicVolume = NewObject<USL_ListDataObject_Scalar>();
			MusicVolume->SetDataID(FName(TEXT("MusicVolume")));
			MusicVolume->SetDataDisplayName(FText::FromString(TEXT("Music Volume")));
			MusicVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for music volume")));
			MusicVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			MusicVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			MusicVolume->SetSliderStepSize(0.01f);
			MusicVolume->SetDefaultValueFromString(LexToString(1.f));
			MusicVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			MusicVolume->SetNumberFormattingOptions(USL_ListDataObject_Scalar::NoDecimal());
			MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMusicVolume));
			MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMusicVolume));
			MusicVolume->SetShouldApplySettingsImmediately(true);
			
			VolumeCategoryCollection->AddChildListData(MusicVolume);
		}
		
		// Sound FX Volume
		{
			USL_ListDataObject_Scalar* SoundFXVolume = NewObject<USL_ListDataObject_Scalar>();
			SoundFXVolume->SetDataID(FName(TEXT("SoundFXVolume")));
			SoundFXVolume->SetDataDisplayName(FText::FromString(TEXT("Sound Effects Volume")));
			SoundFXVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for sound effects volume")));
			SoundFXVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			SoundFXVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			SoundFXVolume->SetSliderStepSize(0.01f);
			SoundFXVolume->SetDefaultValueFromString(LexToString(1.f));
			SoundFXVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			SoundFXVolume->SetNumberFormattingOptions(USL_ListDataObject_Scalar::NoDecimal());
			SoundFXVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSoundFXVolume));
			SoundFXVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSoundFXVolume));
			SoundFXVolume->SetShouldApplySettingsImmediately(true);
			
			VolumeCategoryCollection->AddChildListData(SoundFXVolume);
		}
	}
	
	// Sound Category
	{
		USL_ListDataObject_Collection* SoundCategoryCollection = NewObject<USL_ListDataObject_Collection>();
		SoundCategoryCollection->SetDataID(FName(TEXT("SoundCategoryCollection")));
		SoundCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Sound")));
		
		AudioTabCollection->AddChildListData(SoundCategoryCollection);
		
		// Allow Background Audio
		{
			USL_ListDataObject_StringBool* AllowBackgroundAudio = NewObject<USL_ListDataObject_StringBool>();
			AllowBackgroundAudio->SetDataID(FName(TEXT("AllowBackgroundAudio")));
			AllowBackgroundAudio->SetDataDisplayName(FText::FromString(TEXT("Allow Background Audio")));
			AllowBackgroundAudio->SetFalseAsDefaultValue();
			AllowBackgroundAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackgroundAudio));
			AllowBackgroundAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackgroundAudio));
			AllowBackgroundAudio->SetShouldApplySettingsImmediately(true);
			
			SoundCategoryCollection->AddChildListData(AllowBackgroundAudio);
		}
	}
	
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