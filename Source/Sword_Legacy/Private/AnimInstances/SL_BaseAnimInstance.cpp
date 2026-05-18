#include "AnimInstances/SL_BaseAnimInstance.h"
#include "Utilities/SL_FunctionLibrary.h"

bool USL_BaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	if (APawn* OwningPawn = TryGetPawnOwner())
	{
		return USL_FunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);
	}
	
	return false;
}