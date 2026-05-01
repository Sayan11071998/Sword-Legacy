#include "Characters/SL_EnemyCharacter.h"
#include "Components/Combat/SL_EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASL_EnemyCharacter::ASL_EnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	
	EnemyCombatComponent = CreateDefaultSubobject<USL_EnemyCombatComponent>(TEXT("EnemyCombatComponent"));
}

void ASL_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->HideBoneByName(FName(TEXT("weapon_l")), PBO_Term);
}