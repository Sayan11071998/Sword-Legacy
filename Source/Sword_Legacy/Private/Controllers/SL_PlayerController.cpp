#include "Controllers/SL_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Subsystems/SL_UISubsystem.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"
#include "Widgets/SL_Widget_Activatable_Base.h"
#include "Widgets/SL_Widget_PrimaryLayout.h"

ASL_PlayerController::ASL_PlayerController()
{
	PlayerTeamID = FGenericTeamId(0);
}

FGenericTeamId ASL_PlayerController::GetGenericTeamId() const
{
	return PlayerTeamID;
}

void ASL_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalPlayerController() || !PrimaryLayoutWidgetClass) return;

	USL_Widget_PrimaryLayout* CreatedPrimaryLayout = CreateWidget<USL_Widget_PrimaryLayout>(this, PrimaryLayoutWidgetClass);
	if (!CreatedPrimaryLayout) return;

	CreatedPrimaryLayout->AddToViewport();

	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this);
	UISubsystem->RegisterCreatedPrimaryLayoutWidget(CreatedPrimaryLayout);

	UISubsystem->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_Frontend,
		USL_FunctionLibrary::GetGameSoftWidgetClassByTag(SL_GameplayTags::UI_Widget_PressAnyKeyScreen),
		[this](EAsyncPushWidgetState InPushState, TObjectPtr<USL_Widget_Activatable_Base> PushedWidget)
		{
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				PushedWidget->SetOwningPlayer(this);
			}
		}
	);
}

void ASL_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AActor* CurrentViewTarget = GetViewTarget())
	{
		if (CurrentViewTarget != this && CurrentViewTarget != GetPawn())
		{
			SetViewTarget(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ASL_PlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (!aPawn)
	{
		SetViewTargetToDefaultCamera();
	}
}

void ASL_PlayerController::SetViewTargetToDefaultCamera()
{
	if (!GetWorld() || GetWorld()->bIsTearingDown) return;

	TArray<AActor*> FoundCameras;

	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName("Default"), FoundCameras);

	if (!FoundCameras.IsEmpty() && IsValid(FoundCameras[0]))
	{
		SetViewTarget(FoundCameras[0]);
	}
}