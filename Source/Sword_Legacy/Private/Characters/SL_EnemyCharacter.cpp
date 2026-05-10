#include "Characters/SL_EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/SL_EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartupData/SL_DataAsset_StartupData_Enemy.h"
#include "Items/Weapons/SL_WeaponBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/UI/SL_EnemyUIComponent.h"

ASL_EnemyCharacter::ASL_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	EnemyCombatComponent = CreateDefaultSubobject<USL_EnemyCombatComponent>(TEXT("EnemyCombatComponent"));
	EnemyUIComponent = CreateDefaultSubobject<USL_EnemyUIComponent>(TEXT("EnemyUIComponent"));
}

void ASL_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->HideBoneByName(LeftWeaponBoneName, PBO_Term);
	
	if (DissolveCurve)
	{
		FOnTimelineFloat UpdateDelegate;
		UpdateDelegate.BindUFunction(this, FName("HandleDissolveUpdate"));
		DissolveTimeline.AddInterpFloat(DissolveCurve, UpdateDelegate);
		
		FOnTimelineEvent FinishedDelegate;
		FinishedDelegate.BindUFunction(this, FName("HandleDissolveFinished"));
		DissolveTimeline.SetTimelineFinishedFunc(FinishedDelegate);
	}
}

void ASL_EnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (DissolveTimeline.IsPlaying())
	{
		DissolveTimeline.TickTimeline(DeltaTime);
	}
}

void ASL_EnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitEnemyStartupData();
}

TObjectPtr<USL_PawnCombatComponent> ASL_EnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

void ASL_EnemyCharacter::OnEnemyDied_Implementation(const TSoftObjectPtr<UNiagaraSystem>& InSoftNiagaraSystem)
{
	if (GetMesh())
	{
		GetMesh()->bPauseAnims = true;
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	if (!InSoftNiagaraSystem.IsNull())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			InSoftNiagaraSystem.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[WeakThis = TWeakObjectPtr<ASL_EnemyCharacter>(this), InSoftNiagaraSystem]()
				{
					if (ASL_EnemyCharacter* StrongThis = WeakThis.Get())
					{
						if (UNiagaraSystem* LoadedSystem = InSoftNiagaraSystem.Get())
						{
						UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
								LoadedSystem,
								StrongThis->GetMesh(),
								NAME_None,
								FVector::ZeroVector,
								FRotator::ZeroRotator,
								EAttachLocation::KeepRelativeOffset,
								true
							);

						if (StrongThis->GetMesh())
						{
							UMaterialInstanceDynamic* DynamicMaterial = StrongThis->GetMesh()->CreateDynamicMaterialInstance(0);
							if (DynamicMaterial && NiagaraComp)
							{
								FLinearColor EdgeColor;
								if (DynamicMaterial->GetVectorParameterValue(StrongThis->DissolveEdgeColorName, EdgeColor))
								{
									NiagaraComp->SetVariableLinearColor(StrongThis->DissolveNiagaraParticleColorName, EdgeColor);
								}
							}
						}
						}
					}
				}
			)
		);
	}
	
	if (DissolveCurve)
	{
		const float PlayRate = 1.0f / FMath::Max(TotalDissolveTime, 0.1f);
		DissolveTimeline.SetPlayRate(PlayRate);
		DissolveTimeline.PlayFromStart();
        
		SetActorTickEnabled(true);
	}
}

TObjectPtr<USL_PawnUIComponent> ASL_EnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

void ASL_EnemyCharacter::InitEnemyStartupData()
{
	if (CharacterStartupData.IsNull()) return;
	
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartupData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (USL_DataAsset_StartupData_Base* LoadedData = CharacterStartupData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(CharacterAbilitySystemComponent);
				}
			}
		)
	);
}

void ASL_EnemyCharacter::HandleDissolveUpdate(float Value)
{
	if (GetMesh())
	{
		GetMesh()->SetScalarParameterValueOnMaterials(DissolveParameterName, Value);
	}
	
	if (EnemyCombatComponent)
	{
		if (ASL_WeaponBase* EquippedWeapon = EnemyCombatComponent->GetCharacterCurrentEquippedWeapon())
		{
			if (EquippedWeapon->GetWeaponMesh())
			{
				EquippedWeapon->GetWeaponMesh()->SetScalarParameterValueOnMaterials(DissolveParameterName, Value);
			}
		}
	}
}

void ASL_EnemyCharacter::HandleDissolveFinished()
{
	if (EnemyCombatComponent)
	{
		if (ASL_WeaponBase* EquippedWeapon = EnemyCombatComponent->GetCharacterCurrentEquippedWeapon())
		{
			EquippedWeapon->Destroy();
		}
	}
	
	Destroy();
}