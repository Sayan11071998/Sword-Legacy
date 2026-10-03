#include "Widgets/Options/SL_OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "Utilities/SL_OptionsDataInteractionHelper.h"
#include "Utilities/SL_GameUserSettings.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_StringBool.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_StringEnum.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Resolution.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_StringInteger.h"

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
			AllowBackgroundAudio->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
			AllowBackgroundAudio->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
			AllowBackgroundAudio->SetFalseAsDefaultValue();
			AllowBackgroundAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackgroundAudio));
			AllowBackgroundAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackgroundAudio));
			AllowBackgroundAudio->SetShouldApplySettingsImmediately(true);
			
			SoundCategoryCollection->AddChildListData(AllowBackgroundAudio);
		}
		
		// Use HDR Audio
		{
			USL_ListDataObject_StringBool* UseHDRAudioMode = NewObject<USL_ListDataObject_StringBool>();
			UseHDRAudioMode->SetDataID(FName(TEXT("UseHDRAudioMode")));
			UseHDRAudioMode->SetDataDisplayName(FText::FromString(TEXT("Use HDR Audio Mode")));
			UseHDRAudioMode->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
			UseHDRAudioMode->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
			UseHDRAudioMode->SetFalseAsDefaultValue();
			UseHDRAudioMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetUseHDRAudioMode));
			UseHDRAudioMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetUseHDRAudioMode));
			UseHDRAudioMode->SetShouldApplySettingsImmediately(true);
			
			SoundCategoryCollection->AddChildListData(UseHDRAudioMode);
		}
	}
	
	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void USL_OptionsDataRegistry::InitVideoCollectionTab()
{
	USL_ListDataObject_Collection* VideoTabCollection = NewObject<USL_ListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName(TEXT("VideoTabCollection")));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("Video")));
	
	USL_ListDataObject_StringEnum* CreatedWindowMode = nullptr;
	
	// Display Category
	{
		USL_ListDataObject_Collection* DisplayCategoryCollection = NewObject<USL_ListDataObject_Collection>();
		DisplayCategoryCollection->SetDataID(FName(TEXT("DisplayCategoryCollection")));
		DisplayCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Display")));
		
		VideoTabCollection->AddChildListData(DisplayCategoryCollection);
		
		FSL_OptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
		
		PackagedBuildOnlyCondition.SetEditConditionFunc(
			[]()->bool
			{
				const bool bIsInEditor = GIsEditor || GIsPlayInEditorWorld;
				
				return !bIsInEditor;
			}
		);
		
		PackagedBuildOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This settings can only be adjusted in a packaged build.</>"));
		
		// Window Mode
		{
			USL_ListDataObject_StringEnum* WindowMode = NewObject<USL_ListDataObject_StringEnum>();
			WindowMode->SetDataID(FName(TEXT("WindowMode")));
			WindowMode->SetDataDisplayName(FText::FromString(TEXT("Window Mode")));
			WindowMode->SetDescriptionRichText(FText::FromString(TEXT("This is description for Window Mode")));
			WindowMode->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen Mode")));
			WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Borderless Window")));
			WindowMode->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));
			WindowMode->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
			WindowMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFullscreenMode));
			WindowMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFullscreenMode));
			WindowMode->SetShouldApplySettingsImmediately(true);
			WindowMode->AddEditCondition(PackagedBuildOnlyCondition);
			
			CreatedWindowMode = WindowMode;
			
			DisplayCategoryCollection->AddChildListData(WindowMode);
		}
		
		// Screen Resolution
		{
			USL_ListDataObject_Resolution* ScreenResolution = NewObject<USL_ListDataObject_Resolution>();
			ScreenResolution->SetDataID(FName(TEXT("ScreenResolution")));
			ScreenResolution->SetDataDisplayName(FText::FromString(TEXT("Screen Resolution")));
			ScreenResolution->SetDescriptionRichText(FText::FromString(TEXT("This is description for Screen Resolution")));
			ScreenResolution->InitResolutionValues();
			ScreenResolution->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetScreenResolution));
			ScreenResolution->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetScreenResolution));
			ScreenResolution->SetShouldApplySettingsImmediately(true);
			ScreenResolution->AddEditCondition(PackagedBuildOnlyCondition);
			
			FSL_OptionsDataEditConditionDescriptor WindowModeEditCondition;
			WindowModeEditCondition.SetEditConditionFunc(
				[CreatedWindowMode]() -> bool
				{
					const bool bIsBorderlessWindow = CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::WindowedFullscreen;
					
					return !bIsBorderlessWindow;
				}
			);
			WindowModeEditCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>Screen Resolution is not adjustable when the window mode is set to Borderless Window. The value must match with the maximum allowed resolution.</>"));
			WindowModeEditCondition.SetDisabledForcedStringValue(ScreenResolution->GetMaximumAllowedResolution());
			
			ScreenResolution->AddEditCondition(WindowModeEditCondition);
			
			ScreenResolution->AddEditDependencyData(CreatedWindowMode);
			
			DisplayCategoryCollection->AddChildListData(ScreenResolution);
		}
	}
	
	// Graphics Category
	{
		USL_ListDataObject_Collection* GraphicsCategoryCollection = NewObject<USL_ListDataObject_Collection>();
		
		GraphicsCategoryCollection->SetDataID(FName(TEXT("GraphicsCategoryCollection")));
		GraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Graphics")));
		
		VideoTabCollection->AddChildListData(GraphicsCategoryCollection);
		
		// Display Gamma
		{
			USL_ListDataObject_Scalar* DisplayGamma = NewObject<USL_ListDataObject_Scalar>();
			DisplayGamma->SetDataID(FName(TEXT("DisplayGamma")));
			DisplayGamma->SetDataDisplayName(FText::FromString(TEXT("Brightness")));
			DisplayGamma->SetDescriptionRichText(FText::FromString(TEXT("This is description for brightness")));
			DisplayGamma->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			DisplayGamma->SetOutputValueRange(TRange<float>(1.7f, 2.7f));
			DisplayGamma->SetDisplayNumericType(ECommonNumericType::Percentage);
			DisplayGamma->SetNumberFormattingOptions(USL_ListDataObject_Scalar::NoDecimal());
			DisplayGamma->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentDisplayGamma));
			DisplayGamma->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentDisplayGamma));
			DisplayGamma->SetDefaultValueFromString(LexToString(2.2f));
			
			GraphicsCategoryCollection->AddChildListData(DisplayGamma);
		}
		
		USL_ListDataObject_StringInteger* CreatedOverallQuality = nullptr;
		
		// Overall Quality
		{
			USL_ListDataObject_StringInteger* OverallQuality = NewObject<USL_ListDataObject_StringInteger>();
			OverallQuality->SetDataID(FName(TEXT("OverallQuality")));
			OverallQuality->SetDataDisplayName(FText::FromString(TEXT("Overall Quality")));
			OverallQuality->SetDescriptionRichText(FText::FromString(TEXT("This is description for overall quality")));
			OverallQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			OverallQuality->AddIntegerOption(1, FText::FromString(TEXT("Normal")));
			OverallQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			OverallQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			OverallQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			OverallQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallScalabilityLevel));
			OverallQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallScalabilityLevel));
			OverallQuality->SetShouldApplySettingsImmediately(true);
			
			GraphicsCategoryCollection->AddChildListData(OverallQuality);
			
			CreatedOverallQuality = OverallQuality;
		}
		
		// Resolution Scale
		{
			USL_ListDataObject_Scalar* ResolutionScale = NewObject<USL_ListDataObject_Scalar>();
			ResolutionScale->SetDataID(FName(TEXT("ResolutionScale")));
			ResolutionScale->SetDataDisplayName(FText::FromString(TEXT("3D Resolution")));
			ResolutionScale->SetDescriptionRichText(FText::FromString(TEXT("This is description for resolution scale")));
			ResolutionScale->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetOutputValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetDisplayNumericType(ECommonNumericType::Percentage);
			ResolutionScale->SetNumberFormattingOptions(USL_ListDataObject_Scalar::NoDecimal());
			ResolutionScale->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetResolutionScaleNormalized));
			ResolutionScale->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetResolutionScaleNormalized));
			ResolutionScale->SetShouldApplySettingsImmediately(true);
			ResolutionScale->AddEditDependencyData(CreatedOverallQuality);
			
			GraphicsCategoryCollection->AddChildListData(ResolutionScale);
		}
		
		// Global Illumination Quality
		{
			USL_ListDataObject_StringInteger* GlobalIlluminationQuality = NewObject<USL_ListDataObject_StringInteger>();
			GlobalIlluminationQuality->SetDataID(FName(TEXT("GlobalIlluminationQuality")));
			GlobalIlluminationQuality->SetDataDisplayName(FText::FromString(TEXT("Global Illumination")));
			GlobalIlluminationQuality->SetDescriptionRichText(FText::FromString(TEXT("This is description for global illumination quality")));
			GlobalIlluminationQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			GlobalIlluminationQuality->AddIntegerOption(1, FText::FromString(TEXT("Normal")));
			GlobalIlluminationQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			GlobalIlluminationQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			GlobalIlluminationQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			GlobalIlluminationQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetShouldApplySettingsImmediately(true);
			GlobalIlluminationQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(GlobalIlluminationQuality);
			
			GraphicsCategoryCollection->AddChildListData(GlobalIlluminationQuality);
		}
	}
	
	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void USL_OptionsDataRegistry::InitControlCollectionTab()
{
	USL_ListDataObject_Collection* ControlTabCollection = NewObject<USL_ListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName(TEXT("ControlTabCollection")));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("Control")));
	
	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}