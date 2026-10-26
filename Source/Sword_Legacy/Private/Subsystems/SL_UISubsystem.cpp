#include "Subsystems/SL_UISubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UI/Widgets/SL_Widget_Activatable_Base.h"
#include "UI/Widgets/SL_Widget_PlayerHUD.h"
#include "UI/Widgets/SL_Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "UI/Widgets/Screens/SL_Widget_ConfirmScreen.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

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

void USL_UISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &USL_UISubsystem::HandleWorldBeginTearDown);
}

void USL_UISubsystem::Deinitialize()
{
	ClearGameHUDFade();
	
	FWorldDelegates::OnWorldBeginTearDown.RemoveAll(this);
	
	CreatedPrimaryLayout = nullptr;
	PlayerHUD = nullptr;
	bPlayerHUDPushRequested = false;
	bGameHUDFadeRequested = false;
	
	Super::Deinitialize();
}

bool USL_UISubsystem::HasPrimaryLayout() const
{
	return CreatedPrimaryLayout && IsValid(CreatedPrimaryLayout.Get());
}

void USL_UISubsystem::RemoveWidgetFromStack(const FGameplayTag& InWidgetStackTag, USL_Widget_Activatable_Base* WidgetToRemove)
{
	if (!WidgetToRemove || !HasPrimaryLayout()) return;

	if (UCommonActivatableWidgetContainerBase* WidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag))
	{
		WidgetStack->RemoveWidget(*WidgetToRemove);
	}
}

void USL_UISubsystem::HandleWorldBeginTearDown(UWorld* World)
{
	if (CreatedPrimaryLayout && CreatedPrimaryLayout->GetWorld() == World)
	{
		ClearGameHUDFade();
		
		CreatedPrimaryLayout = nullptr;
		PlayerHUD = nullptr;
		bPlayerHUDPushRequested = false;
		bGameHUDFadeRequested = false;
	}
}

void USL_UISubsystem::PushPlayerHUD(TSoftClassPtr<USL_Widget_Activatable_Base> PlayerHUDClass)
{
	if (bPlayerHUDPushRequested || IsValid(PlayerHUD)) return;
	if (PlayerHUDClass.IsNull() || !HasPrimaryLayout()) return;

	UClass* LoadedClass = PlayerHUDClass.LoadSynchronous();
	
	if (!LoadedClass || !LoadedClass->IsChildOf(USL_Widget_PlayerHUD::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player HUD widget must use SL_Widget_PlayerHUD as its parent."));
		
		return;
	}

	if (UCommonActivatableWidgetContainerBase* WidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(SL_GameplayTags::UI_WidgetStack_GameHUD))
	{
		WidgetStack->SetTransitionDuration(0.f);
		WidgetStack->SetRenderOpacity(0.f);
	}

	bPlayerHUDPushRequested = true;

	PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_GameHUD,
		PlayerHUDClass,
		[this](EAsyncPushWidgetState InPushState, TObjectPtr<USL_Widget_Activatable_Base> PushedWidget)
		{
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				SetGameHUDOpacity(0.f);
			}

			if (InPushState == EAsyncPushWidgetState::AfterPush)
			{
				PlayerHUD = PushedWidget;
				SetGameHUDOpacity(0.f);

				if (bGameHUDFadeRequested)
				{
					StartGameHUDFade();
				}
			}
		}
	);
}

void USL_UISubsystem::RemovePlayerHUD()
{
	if (!IsValid(PlayerHUD))
	{
		PlayerHUD = nullptr;
		
		return;
	}

	RemoveWidgetFromStack(SL_GameplayTags::UI_WidgetStack_GameHUD, PlayerHUD);
	
	PlayerHUD = nullptr;
	bPlayerHUDPushRequested = false;
}

void USL_UISubsystem::FadeGameHUD(float TargetOpacity, float Duration)
{
	GameHUDFadeTargetOpacity = FMath::Clamp(TargetOpacity, 0.f, 1.f);
	GameHUDFadeDuration = FMath::Max(Duration, 0.f);
	bGameHUDFadeRequested = true;

	if (!GetGameHUDStack()) return;

	StartGameHUDFade();
}

void USL_UISubsystem::RegisterCreatedPrimaryLayoutWidget(USL_Widget_PrimaryLayout* InCreatedWidget)
{
	check(InCreatedWidget);
	
	CreatedPrimaryLayout = InCreatedWidget;
}

void USL_UISubsystem::SetGameHUDOpacity(float Opacity)
{
	if (UCommonActivatableWidgetContainerBase* WidgetStack = GetGameHUDStack())
	{
		WidgetStack->SetRenderOpacity(Opacity);
	}
}

void USL_UISubsystem::StartGameHUDFade()
{
	UCommonActivatableWidgetContainerBase* WidgetStack = GetGameHUDStack();
	UWorld* World = WidgetStack ? WidgetStack->GetWorld() : nullptr;
	
	if (!WidgetStack || !World) return;

	World->GetTimerManager().ClearTimer(GameHUDFadeTimer);

	if (GameHUDFadeDuration <= 0.f)
	{
		WidgetStack->SetRenderOpacity(GameHUDFadeTargetOpacity);
		
		return;
	}

	GameHUDFadeStartOpacity = WidgetStack->GetRenderOpacity();
	GameHUDFadeStartTime = World->GetTimeSeconds();

	World->GetTimerManager().SetTimer(
		GameHUDFadeTimer,
		this,
		&USL_UISubsystem::TickGameHUDFade,
		1.f / 60.f,
		true
	);
}

UCommonActivatableWidgetContainerBase* USL_UISubsystem::GetGameHUDStack() const
{
	if (!HasPrimaryLayout()) return nullptr;

	return CreatedPrimaryLayout->FindWidgetStackByTag(SL_GameplayTags::UI_WidgetStack_GameHUD);
}

void USL_UISubsystem::ClearGameHUDFade()
{
	if (CreatedPrimaryLayout)
	{
		if (UWorld* World = CreatedPrimaryLayout->GetWorld())
		{
			World->GetTimerManager().ClearTimer(GameHUDFadeTimer);
		}
	}
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
				if (!LoadedWidgetClass || !IsValid(CreatedPrimaryLayout.Get()))
				{
					UE_LOG(LogTemp, Warning, TEXT("Cannot push widget. Layout or widget class is not valid."));
					return;
				}
				
				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
				if (!FoundWidgetStack)
				{
					UE_LOG(LogTemp, Warning, TEXT("Cannot find widget stack %s"), *InWidgetStackTag.ToString());
					return;
				}
				
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

void USL_UISubsystem::PushConfirmScreenToModalStackAsync(ESL_ConfirmScreenType InScreenType, const FText& InScreenTitle,
	const FText& InScreenMsg, TFunction<void(ESL_ConfirmScreenButtonType)> ButtonClickedCallback)
{
	USL_ConfirmScreenInfoObject* CreatedInfoObject = nullptr;
	
	switch (InScreenType)
	{
	case ESL_ConfirmScreenType::Ok:
		CreatedInfoObject = USL_ConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScreenMsg);
		break;
		
	case ESL_ConfirmScreenType::YesNo:
		CreatedInfoObject = USL_ConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMsg);
		break;
		
	case ESL_ConfirmScreenType::OkCancel:
		CreatedInfoObject = USL_ConfirmScreenInfoObject::CreateOkCancelScreen(InScreenTitle, InScreenMsg);
		break;
		
	case ESL_ConfirmScreenType::Unknown:
		break;
		
	default:
		break;
	}
	
	check(CreatedInfoObject);
	
	PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_Modal,
		USL_FunctionLibrary::GetGameSoftWidgetClassByTag(SL_GameplayTags::UI_Widget_ConfirmScreen),
		[CreatedInfoObject, ButtonClickedCallback](EAsyncPushWidgetState InPushState, USL_Widget_Activatable_Base* PushedWidget)
		{
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				USL_Widget_ConfirmScreen* CreatedConfirmScreen = CastChecked<USL_Widget_ConfirmScreen>(PushedWidget);
				
				CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
			}
		}
	);
}

void USL_UISubsystem::TickGameHUDFade()
{
	UCommonActivatableWidgetContainerBase* WidgetStack = GetGameHUDStack();
	UWorld* World = WidgetStack ? WidgetStack->GetWorld() : nullptr;
	
	if (!WidgetStack || !World)
	{
		ClearGameHUDFade();
		
		return;
	}

	const float Alpha = FMath::Clamp((World->GetTimeSeconds() - GameHUDFadeStartTime) / GameHUDFadeDuration, 0.f, 1.f);
	const float EasedAlpha = Alpha * Alpha * (3.f - 2.f * Alpha);

	WidgetStack->SetRenderOpacity(FMath::Lerp(GameHUDFadeStartOpacity, GameHUDFadeTargetOpacity, EasedAlpha));

	if (Alpha >= 1.f)
	{
		World->GetTimerManager().ClearTimer(GameHUDFadeTimer);
	}
}