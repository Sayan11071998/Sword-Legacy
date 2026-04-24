#include "Characters/SL_BaseCharacter.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/SL_AttributeSet.h"

ASL_BaseCharacter::ASL_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	GetMesh()->bReceivesDecals = true;
	
	CharacterAbilitySystemComponent = CreateDefaultSubobject<USL_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	CharacterAttributeSet = CreateDefaultSubobject<USL_AttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ASL_BaseCharacter::GetAbilitySystemComponent() const
{
	return GetCharacterAbilitySystemComponent();
}

void ASL_BaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (CharacterAbilitySystemComponent)
	{
		CharacterAbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		ensureMsgf(!CharacterStartupData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}