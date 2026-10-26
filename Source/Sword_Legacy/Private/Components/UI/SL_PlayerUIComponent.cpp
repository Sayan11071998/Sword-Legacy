#include "Components/UI/SL_PlayerUIComponent.h"
#include "AbilitySystem/SL_AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Characters/SL_BaseCharacter.h"
#include "Subsystems/SL_UISubsystem.h"

void USL_PlayerUIComponent::RegisterPlayerDrawnWidget(UUserWidget* InWidgetToRegister)
{
	if (InWidgetToRegister)
	{
		PlayerDrawnWidgets.AddUnique(InWidgetToRegister);
	}
}

void USL_PlayerUIComponent::RemovePlayerDrawnWidgetsIfAny()
{
	for (UUserWidget* DrawnWidget : PlayerDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();
		}
	}
	
	PlayerDrawnWidgets.Empty();

	if (USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(this))
	{
		UISubsystem->RemovePlayerHUD();
	}
}

void USL_PlayerUIComponent::BroadcastCurrentHealthAndRage()
{
	const ASL_BaseCharacter* Character = GetOwner<ASL_BaseCharacter>();
	const USL_AttributeSet* Attributes = Character ? Character->GetCharacterAttributeSet() : nullptr;
	
	if (!Attributes) return;

	const float MaxHealth = Attributes->GetMaxHealth();
	
	if (MaxHealth > 0.f)
	{
		OnCurrentHealthChanged.Broadcast(Attributes->GetCurrentHealth() / MaxHealth);
	}

	const float MaxRage = Attributes->GetMaxRage();
	
	if (MaxRage > 0.f)
	{
		OnCurrentRageChanged.Broadcast(Attributes->GetCurrentRage() / MaxRage);
	}
}