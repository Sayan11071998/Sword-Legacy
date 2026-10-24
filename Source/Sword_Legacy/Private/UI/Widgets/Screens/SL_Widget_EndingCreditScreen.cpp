#include "UI/Widgets/Screens/SL_Widget_EndingCreditScreen.h"
#include "CommonRichTextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_Widget_EndingCreditScreen::USL_Widget_EndingCreditScreen()
{
	bIsModal = true;
	bIsBackHandler = false;
	bSupportsActivationFocus = false;
}

void USL_Widget_EndingCreditScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ScrollBox_Main)
	{
		ScrollBox_Main->SetScrollBarVisibility(ESlateVisibility::Collapsed);
		ScrollBox_Main->SetScrollOffset(0.f);
	}

	if (CommonRichTextBlock_Content)
	{
		CommonRichTextBlock_Content->SetText(FText::FromString(USL_FunctionLibrary::GetCreditsDisplayString()));
	}
}

void USL_Widget_EndingCreditScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	ElapsedDelay = 0.f;
	bScrollFinished = false;

	if (ScrollBox_Main)
	{
		ScrollBox_Main->SetScrollOffset(0.f);
	}
}

void USL_Widget_EndingCreditScreen::MakeScrollBoxFillParent() const
{
	UWidget* Widget = ScrollBox_Main;
	
	while (Widget)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			CanvasSlot->SetOffsets(FMargin(0.f));
			CanvasSlot->SetAlignment(FVector2D::ZeroVector);
			CanvasSlot->SetAutoSize(false);
		}
		else if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
		{
			OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
			OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		}
		else if (UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
		{
			VerticalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			VerticalSlot->SetHorizontalAlignment(HAlign_Fill);
			VerticalSlot->SetVerticalAlignment(VAlign_Fill);
		}

		Widget = Widget->GetParent();
	}
}

void USL_Widget_EndingCreditScreen::PadScrollContent(float ViewHeight) const
{
	for (UWidget* Child : ScrollBox_Main->GetAllChildren())
	{
		UScrollBoxSlot* ChildSlot = Cast<UScrollBoxSlot>(Child ? Child->Slot : nullptr);
		
		if (!ChildSlot) continue;

		ChildSlot->SetVerticalAlignment(VAlign_Top);
		ChildSlot->SetHorizontalAlignment(HAlign_Fill);
		ChildSlot->SetPadding(FMargin(0.f, ViewHeight, 0.f, ViewHeight));
	}
}

void USL_Widget_EndingCreditScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bScrollFinished || !ScrollBox_Main) return;

	if (!bScrollBoxFilled)
	{
		MakeScrollBoxFillParent();
		bScrollBoxFilled = true;
		
		return;
	}

	if (!bLayoutPrepared)
	{
		const float ViewHeight = ScrollBox_Main->GetCachedGeometry().GetLocalSize().Y;
		if (ViewHeight <= 1.f) return;

		PadScrollContent(ViewHeight);
		ScrollBox_Main->SetScrollOffset(0.f);
		bLayoutPrepared = true;
		
		return;
	}

	const float EndOffset = ScrollBox_Main->GetScrollOffsetOfEnd();
	if (EndOffset <= 1.f) return;

	ElapsedDelay += InDeltaTime;
	if (ElapsedDelay < ScrollStartDelay) return;

	const float NewOffset = ScrollBox_Main->GetScrollOffset() + (ScrollSpeed * InDeltaTime);
	if (NewOffset >= EndOffset)
	{
		ScrollBox_Main->SetScrollOffset(EndOffset);
		bScrollFinished = true;
		UE_LOG(LogTemp, Display, TEXT("Ending credits finished."));
		
		return;
	}

	ScrollBox_Main->SetScrollOffset(NewOffset);
}

TOptional<FUIInputConfig> USL_Widget_EndingCreditScreen::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}