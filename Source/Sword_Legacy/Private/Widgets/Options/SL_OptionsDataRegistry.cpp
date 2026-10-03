#include "Widgets/Options/SL_OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "Utilities/SL_OptionsDataInteractionHelper.h"
#include "Utilities/SL_GameUserSettings.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_StringBool.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_StringEnum.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Resolution.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_StringInteger.h"
#include "Internationalization/StringTableRegistry.h"

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
	MakeShared<FSL_OptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(USL_GameUserSettings, SetterOrGetterFuncName))

#define GET_GAMEPLAY_TAB_DESCRIPTION(InKey) LOCTABLE("/Game/Game/UI/StringTables/ST_GameplayScreenDescription.ST_GameplayScreenDescription", InKey)

#define GET_AUDIO_TAB_DESCRIPTION(InKey) LOCTABLE("/Game/Game/UI/StringTables/ST_AudioScreenDescription.ST_AudioScreenDescription", InKey)

#define GET_OPTIONS_TAB_DESCRIPTION(InKey) LOCTABLE("/Game/Game/UI/StringTables/ST_OptionsScreenDescription.ST_OptionsScreenDescription", InKey)

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
		GameDifficulty->SetDescriptionRichText(GET_GAMEPLAY_TAB_DESCRIPTION("GameDifficultyDescKey"));
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
			OverallVolume->SetDataDisplayName(FText::FromString(TEXT("Master Volume")));
			OverallVolume->SetDescriptionRichText(GET_AUDIO_TAB_DESCRIPTION("OverallVolumeDescKey"));
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
			MusicVolume->SetDescriptionRichText(GET_AUDIO_TAB_DESCRIPTION("MusicVolumeDescKey"));
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
			SoundFXVolume->SetDataDisplayName(FText::FromString(TEXT("Effects")));
			SoundFXVolume->SetDescriptionRichText(GET_AUDIO_TAB_DESCRIPTION("SoundFXVolumeDescKey"));
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
			AllowBackgroundAudio->SetDataDisplayName(FText::FromString(TEXT("Background Audio")));
			AllowBackgroundAudio->SetDescriptionRichText(GET_AUDIO_TAB_DESCRIPTION("AllowBackgroundAudioDescKey"));
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
			UseHDRAudioMode->SetDataDisplayName(FText::FromString(TEXT("Night Mode")));
			UseHDRAudioMode->SetDescriptionRichText(GET_AUDIO_TAB_DESCRIPTION("UseHDRAudioModeDescKey"));
			UseHDRAudioMode->OverrideTrueDisplayText(FText::FromString(TEXT("ON")));
			UseHDRAudioMode->OverrideFalseDisplayText(FText::FromString(TEXT("OFF")));
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
			WindowMode->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("WindowModeDescKey"));
			WindowMode->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen")));
			WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Windowed Fullscreen")));
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
			ScreenResolution->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("ScreenResolutionsDescKey"));
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
			WindowModeEditCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>Screen Resolution is not adjustable when the window mode is set to Windowed Fullscreen. The value must match with the maximum allowed resolution.</>"));
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
			DisplayGamma->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("DisplayGammaDescKey"));
			DisplayGamma->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			DisplayGamma->SetOutputValueRange(TRange<float>(1.7f, 2.7f));
			DisplayGamma->SetSliderStepSize(0.01f);
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
			OverallQuality->SetDataDisplayName(FText::FromString(TEXT("Graphics Quality")));
			OverallQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("OverallQualityDescKey"));
			OverallQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			OverallQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			OverallQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			OverallQuality->AddIntegerOption(3, FText::FromString(TEXT("Ultra")));
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
			ResolutionScale->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("ResolutionScaleDescKey"));
			ResolutionScale->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetOutputValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetSliderStepSize(0.01f);
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
			GlobalIlluminationQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("GlobalIlluminationQualityDescKey"));
			GlobalIlluminationQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			GlobalIlluminationQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			GlobalIlluminationQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			GlobalIlluminationQuality->AddIntegerOption(3, FText::FromString(TEXT("Ultra")));
			GlobalIlluminationQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			GlobalIlluminationQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetShouldApplySettingsImmediately(true);
			GlobalIlluminationQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(GlobalIlluminationQuality);
			
			GraphicsCategoryCollection->AddChildListData(GlobalIlluminationQuality);
		}
		
		// Shadow Quality
		{
			USL_ListDataObject_StringInteger* ShadowQuality = NewObject<USL_ListDataObject_StringInteger>();
			ShadowQuality->SetDataID(FName(TEXT("ShadowQuality")));
			ShadowQuality->SetDataDisplayName(FText::FromString(TEXT("Shadow Quality")));
			ShadowQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("ShadowQualityDescKey"));
			ShadowQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			ShadowQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			ShadowQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			ShadowQuality->AddIntegerOption(3, FText::FromString(TEXT("Ultra")));
			ShadowQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			ShadowQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShadowQuality));
			ShadowQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShadowQuality));
			ShadowQuality->SetShouldApplySettingsImmediately(true);
			ShadowQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(ShadowQuality);
			
			GraphicsCategoryCollection->AddChildListData(ShadowQuality);
		}
		
		// Anti-Aliasing Quality
		{
			USL_ListDataObject_StringInteger* AntiAliasingQuality = NewObject<USL_ListDataObject_StringInteger>();
			AntiAliasingQuality->SetDataID(FName(TEXT("AntiAliasingQuality")));
			AntiAliasingQuality->SetDataDisplayName(FText::FromString(TEXT("Anti-Aliasing")));
			AntiAliasingQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("AntiAliasingDescKey"));
			AntiAliasingQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			AntiAliasingQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			AntiAliasingQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			AntiAliasingQuality->AddIntegerOption(3, FText::FromString(TEXT("Ultra")));
			AntiAliasingQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			AntiAliasingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAntiAliasingQuality));
			AntiAliasingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAntiAliasingQuality));
			AntiAliasingQuality->SetShouldApplySettingsImmediately(true);
			AntiAliasingQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(AntiAliasingQuality);
			
			GraphicsCategoryCollection->AddChildListData(AntiAliasingQuality);
		}
		
		// View Distance Quality
		{
			USL_ListDataObject_StringInteger* ViewDistanceQuality = NewObject<USL_ListDataObject_StringInteger>();
			ViewDistanceQuality->SetDataID(FName(TEXT("ViewDistanceQuality")));
			ViewDistanceQuality->SetDataDisplayName(FText::FromString(TEXT("View Distance")));
			ViewDistanceQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("ViewDistanceDescKey"));
			ViewDistanceQuality->AddIntegerOption(0, FText::FromString(TEXT("Near")));
			ViewDistanceQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			ViewDistanceQuality->AddIntegerOption(2, FText::FromString(TEXT("Far")));
			ViewDistanceQuality->AddIntegerOption(3, FText::FromString(TEXT("Very Far")));
			ViewDistanceQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			ViewDistanceQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetViewDistanceQuality));
			ViewDistanceQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetViewDistanceQuality));
			ViewDistanceQuality->SetShouldApplySettingsImmediately(true);
			ViewDistanceQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(ViewDistanceQuality);
			
			GraphicsCategoryCollection->AddChildListData(ViewDistanceQuality);
		}
		
		// Texture Quality
		{
			USL_ListDataObject_StringInteger* TextureQuality = NewObject<USL_ListDataObject_StringInteger>();
			TextureQuality->SetDataID(FName(TEXT("TextureQuality")));
			TextureQuality->SetDataDisplayName(FText::FromString(TEXT("Texture Quality")));
			TextureQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("TextureQualityDescKey"));
			TextureQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			TextureQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			TextureQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			TextureQuality->AddIntegerOption(3, FText::FromString(TEXT("Ultra")));
			TextureQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			TextureQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetTextureQuality));
			TextureQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetTextureQuality));
			TextureQuality->SetShouldApplySettingsImmediately(true);
			TextureQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(TextureQuality);
			
			GraphicsCategoryCollection->AddChildListData(TextureQuality);
		}
		
		// Visual Effects Quality
		{
			USL_ListDataObject_StringInteger* VisualEffectsQuality = NewObject<USL_ListDataObject_StringInteger>();
			VisualEffectsQuality->SetDataID(FName(TEXT("VisualEffectsQuality")));
			VisualEffectsQuality->SetDataDisplayName(FText::FromString(TEXT("Visual Effects Quality")));
			VisualEffectsQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("VisualEffectQualityDescKey"));
			VisualEffectsQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			VisualEffectsQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			VisualEffectsQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			VisualEffectsQuality->AddIntegerOption(3, FText::FromString(TEXT("Ultra")));
			VisualEffectsQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			VisualEffectsQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVisualEffectQuality));
			VisualEffectsQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVisualEffectQuality));
			VisualEffectsQuality->SetShouldApplySettingsImmediately(true);
			VisualEffectsQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(VisualEffectsQuality);
			
			GraphicsCategoryCollection->AddChildListData(VisualEffectsQuality);
		}
		
		// Reflection Quality
		{
			USL_ListDataObject_StringInteger* ReflectionQuality = NewObject<USL_ListDataObject_StringInteger>();
			ReflectionQuality->SetDataID(FName(TEXT("ReflectionQuality")));
			ReflectionQuality->SetDataDisplayName(FText::FromString(TEXT("Reflection Quality")));
			ReflectionQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("ReflectionQualityDescKey"));
			ReflectionQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			ReflectionQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			ReflectionQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			ReflectionQuality->AddIntegerOption(3, FText::FromString(TEXT("Ultra")));
			ReflectionQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			ReflectionQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetReflectionQuality));
			ReflectionQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetReflectionQuality));
			ReflectionQuality->SetShouldApplySettingsImmediately(true);
			ReflectionQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(ReflectionQuality);
			
			GraphicsCategoryCollection->AddChildListData(ReflectionQuality);
		}
		
		// Post Processing Quality
		{
			USL_ListDataObject_StringInteger* PostProcessingQuality = NewObject<USL_ListDataObject_StringInteger>();
			PostProcessingQuality->SetDataID(FName(TEXT("PostProcessingQuality")));
			PostProcessingQuality->SetDataDisplayName(FText::FromString(TEXT("Post-Processing")));
			PostProcessingQuality->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("PostProcessingQualityDescKey"));
			PostProcessingQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			PostProcessingQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			PostProcessingQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			PostProcessingQuality->AddIntegerOption(3, FText::FromString(TEXT("Ultra")));
			PostProcessingQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			PostProcessingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetPostProcessingQuality));
			PostProcessingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetPostProcessingQuality));
			PostProcessingQuality->SetShouldApplySettingsImmediately(true);
			PostProcessingQuality->AddEditDependencyData(CreatedOverallQuality);
			
			CreatedOverallQuality->AddEditDependencyData(PostProcessingQuality);
			
			GraphicsCategoryCollection->AddChildListData(PostProcessingQuality);
		}
	}
	
	// Advanced Graphics Category
	{
		USL_ListDataObject_Collection* AdvancedGraphicsCategoryCollection = NewObject<USL_ListDataObject_Collection>();
		AdvancedGraphicsCategoryCollection->SetDataID(FName(TEXT("AdvancedGraphicsCategoryCollection")));
		AdvancedGraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Advanced Graphics")));
		
		VideoTabCollection->AddChildListData(AdvancedGraphicsCategoryCollection);
		
		// Vertical Sync
		{
			USL_ListDataObject_StringBool* VerticalSync = NewObject<USL_ListDataObject_StringBool>();
			VerticalSync->SetDataID(FName(TEXT("VerticalSync")));
			VerticalSync->SetDataDisplayName(FText::FromString(TEXT("V-Sync")));
			VerticalSync->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("VerticalSyncDescKey"));
			VerticalSync->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(IsVSyncEnabled));
			VerticalSync->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVSyncEnabled));
			VerticalSync->SetFalseAsDefaultValue();
			VerticalSync->SetShouldApplySettingsImmediately(true);
			
			FSL_OptionsDataEditConditionDescriptor FullScreenOnlyCondition;
			FullScreenOnlyCondition.SetEditConditionFunc(
				[CreatedWindowMode]() -> bool
				{
					return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
				}
			);
			FullScreenOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This feature only works if the 'Window Mode' is set to 'Fullscreen'.</>"));
			FullScreenOnlyCondition.SetDisabledForcedStringValue(TEXT("false"));
			
			VerticalSync->AddEditCondition(FullScreenOnlyCondition);
			
			AdvancedGraphicsCategoryCollection->AddChildListData(VerticalSync);
		}
		
		// Frame Rate Limit
		{
			USL_ListDataObject_String* FrameRateLimit = NewObject<USL_ListDataObject_String>();
			FrameRateLimit->SetDataID(FName(TEXT("FrameRateLimit")));
			FrameRateLimit->SetDataDisplayName(FText::FromString(TEXT("Frame Rate Limit")));
			FrameRateLimit->SetDescriptionRichText(GET_OPTIONS_TAB_DESCRIPTION("FrameRateLimitDescKey"));
			FrameRateLimit->AddDynamicOption(LexToString(30.f), FText::FromString(TEXT("30 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(60.f), FText::FromString(TEXT("60 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(90.f), FText::FromString(TEXT("90 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(120.f), FText::FromString(TEXT("120 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(0.f), FText::FromString(TEXT("No Limit")));
			FrameRateLimit->SetDefaultValueFromString(LexToString(0.f));
			FrameRateLimit->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFrameRateLimit));
			FrameRateLimit->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFrameRateLimit));
			FrameRateLimit->SetShouldApplySettingsImmediately(true);
			
			AdvancedGraphicsCategoryCollection->AddChildListData(FrameRateLimit);
		}
	}
	
	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void USL_OptionsDataRegistry::InitControlCollectionTab()
{
	USL_ListDataObject_Collection* ControlTabCollection = NewObject<USL_ListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName(TEXT("ControlTabCollection")));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("Controls")));
	
	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}