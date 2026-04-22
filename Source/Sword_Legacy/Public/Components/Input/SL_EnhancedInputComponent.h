#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/SL_DataAsset_InputConfig.h"
#include "SL_EnhancedInputComponent.generated.h"

struct FGameplayTag;
class USL_DataAsset_InputConfig;

UCLASS()
class SWORD_LEGACY_API USL_EnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserObject, typename CallbackFunc>
	void BindNativeInputAction(
		const TObjectPtr<USL_DataAsset_InputConfig> InInputConfig,
		const FGameplayTag& InInputTag,
		ETriggerEvent TriggerEvent,
		TObjectPtr<UserObject> ContextObject,
		CallbackFunc Func
	);
};

template <class UserObject, typename CallbackFunc>
void USL_EnhancedInputComponent::BindNativeInputAction(const TObjectPtr<USL_DataAsset_InputConfig> InInputConfig,
	const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, TObjectPtr<UserObject> ContextObject, CallbackFunc Func)
{
	checkf(InInputConfig, TEXT("Input Config Data Asset is Null. Can't Proceed with Binding"));
	
	if (UInputAction* FoundAction = InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}
}