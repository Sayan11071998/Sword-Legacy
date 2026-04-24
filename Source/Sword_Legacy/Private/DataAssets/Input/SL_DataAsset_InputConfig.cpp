#include "DataAssets/Input/SL_DataAsset_InputConfig.h"

TObjectPtr<UInputAction> USL_DataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FSL_InputActionConfig InputActionConfig : NativeInputActions)
	{
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	
	return nullptr;
}