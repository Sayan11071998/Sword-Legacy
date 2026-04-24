#include "Characters/SL_PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Input/SL_EnhancedInputComponent.h"
#include "DataAssets/Input/SL_DataAsset_InputConfig.h"
#include "Utilities/SL_GameplayTags.h"
#include "DataAssets/StartupData/SL_DataAsset_StartupData_Player.h"

ASL_PlayerCharacter::ASL_PlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	PlayerCombatComponent = CreateDefaultSubobject<USL_PlayerCombatComponent>(TEXT("PlayerCombatComponent"));
}

void ASL_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASL_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("Forgot to Assign a Valid Data Asset as Input Config"));
	
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);
	
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
	USL_EnhancedInputComponent* EnhancedInputComponent = CastChecked<USL_EnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, SL_GameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ASL_PlayerCharacter::Input_Move);
	EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, SL_GameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ASL_PlayerCharacter::Input_Look);

	EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, SL_GameplayTags::InputTag_Sprint, ETriggerEvent::Triggered, this, &ASL_PlayerCharacter::Input_SprintStarted);
	EnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, SL_GameplayTags::InputTag_Sprint, ETriggerEvent::Completed, this, &ASL_PlayerCharacter::Input_SprintCompleted);
}

void ASL_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!CharacterStartupData.IsNull())
	{
		if (USL_DataAsset_StartupData_Base* LoadedData = CharacterStartupData.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(CharacterAbilitySystemComponent);
		}
	}
}

void ASL_PlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	
	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}
	
	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASL_PlayerCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVectorVector = InputActionValue.Get<FVector2D>();
	
	if (LookAxisVectorVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVectorVector.X);
	}
	
	if (LookAxisVectorVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVectorVector.Y);
	}
}

void ASL_PlayerCharacter::Input_SprintStarted(const FInputActionValue& InputActionValue)
{
	if (GetCharacterMovement()->GetCurrentAcceleration().SizeSquared2D() > 0.f)
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
	}
}

void ASL_PlayerCharacter::Input_SprintCompleted(const FInputActionValue& InputActionValue)
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}