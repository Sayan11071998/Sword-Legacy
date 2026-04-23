#include "Characters/SL_BaseCharacter.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/SL_AttributeSet.h"

ASL_BaseCharacter::ASL_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	GetMesh()->bReceivesDecals = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<USL_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<USL_AttributeSet>(TEXT("AttributeSet"));
}