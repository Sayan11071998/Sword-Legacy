#include "AnimInstances/Player/SL_PlayerLinkedAnimLayer.h"
#include "AnimInstances/Player/SL_PlayerAnimInstance.h"

USL_PlayerAnimInstance* USL_PlayerLinkedAnimLayer::GetPlayerAnimInstance() const
{
	return Cast<USL_PlayerAnimInstance>(GetOwningComponent()->GetAnimInstance());
}