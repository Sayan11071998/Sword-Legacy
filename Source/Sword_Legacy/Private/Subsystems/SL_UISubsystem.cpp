#include "Subsystems/SL_UISubsystem.h"
#include "Engine/AssetManager.h"
#include "Widgets/SL_Widget_Activatable_Base.h"
#include "Widgets/SL_Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

TObjectPtr<USL_UISubsystem> USL_UISubsystem::Get(const TObjectPtr<UObject> WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		
		return UGameInstance::GetSubsystem<USL_UISubsystem>(World->GetGameInstance());
	}
	
	return nullptr;
}

bool USL_UISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		
		GetDerivedClasses(GetClass(), FoundClasses);
		
		return FoundClasses.IsEmpty();
	}
	
	return false;
}

void USL_UISubsystem::PushSoftWidgetToStackAsync(
	const FGameplayTag& InWidgetStackTag,
	TSoftClassPtr<USL_Widget_Activatable_Base> InSoftWidgetClass,
	TFunction<void(EAsyncPushWidgetState, TObjectPtr<USL_Widget_Activatable_Base>)> AsyncPushStateCallback)
{
	check(!InSoftWidgetClass.IsNull());
	
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[InSoftWidgetClass, InWidgetStackTag, AsyncPushStateCallback, this]()
			{
				UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
				
				check(LoadedWidgetClass && CreatedPrimaryLayout);
				
				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
				
				USL_Widget_Activatable_Base* CreatedWidget = FoundWidgetStack->AddWidget<USL_Widget_Activatable_Base>(
					LoadedWidgetClass,
					[AsyncPushStateCallback](USL_Widget_Activatable_Base& CreatedWidgetInstance)
					{
						AsyncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
					}
				);
				
				AsyncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);
			}
		)
	);
}

void USL_UISubsystem::RegisterCreatedPrimaryLayoutWidget(USL_Widget_PrimaryLayout* InCreatedWidget)
{
	check(InCreatedWidget);
	
	CreatedPrimaryLayout = InCreatedWidget;
}