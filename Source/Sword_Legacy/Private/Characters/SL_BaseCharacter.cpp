#include "Characters/SL_BaseCharacter.h"

ASL_BaseCharacter::ASL_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	GetMesh()->bReceivesDecals = true;
}