#include "Widgets/SL_Widget_PrimaryLayout.h"

#include "SL_DebugHelper.h"

TObjectPtr<UCommonActivatableWidgetContainerBase> USL_Widget_PrimaryLayout::FindWidgetStackByTag(
	const FGameplayTag& InTag) const
{
	checkf(RegisteredWidgetStackMap.Contains(InTag), TEXT("Can not find the widget stack by the tag %s"), *InTag.ToString());
	
	return RegisteredWidgetStackMap.FindRef(InTag);
}

void USL_Widget_PrimaryLayout::RegisterWidgetStack(FGameplayTag InStackTag,
	UCommonActivatableWidgetContainerBase* InStack)
{
	if (IsDesignTime() || !InStack) return;

	if (!RegisteredWidgetStackMap.Contains(InStackTag))
	{
		RegisteredWidgetStackMap.Add(InStackTag, InStack);

		Debug::Print(TEXT("Widget Stack Register under the tag ") + InStackTag.ToString());
	}
}