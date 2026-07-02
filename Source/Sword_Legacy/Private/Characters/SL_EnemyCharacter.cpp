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
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/UI/SL_EnemyUIComponent.h"
#include "Widgets/SL_WidgetBase.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Utilities/SL_GameplayTags.h"

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
	
	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCollisionBox"));
	LeftHandCollisionBox->SetupAttachment(GetMesh());
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASL_EnemyCharacter::OnBodyCollisionBoxBeginOverlap);
	
	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollisionBox"));
	RightHandCollisionBox->SetupAttachment(GetMesh());
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASL_EnemyCharacter::OnBodyCollisionBoxBeginOverlap);
	
	EnemyUIComponent = CreateDefaultSubobject<USL_EnemyUIComponent>(TEXT("EnemyUIComponent"));
	
	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthWidgetComponent"));
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());
}

void ASL_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->HideBoneByName(LeftWeaponBoneName, PBO_Term);
	
	if (USL_WidgetBase* HealthWidget = Cast<USL_WidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		HealthWidget->InitEnemyCreatedWidget(this);
	}
	
	if (DissolveCurve)
	{
		FOnTimelineFloat UpdateDelegate;
		UpdateDelegate.BindUFunction(this, FName("HandleDissolveUpdate"));
		DissolveTimeline.AddInterpFloat(DissolveCurve, UpdateDelegate);
		
		FOnTimelineEvent FinishedDelegate;
		FinishedDelegate.BindUFunction(this, FName("HandleDissolveFinished"));
		DissolveTimeline.SetTimelineFinishedFunc(FinishedDelegate);
	}

	if (EntryRestoreCurve)
	{
		FOnTimelineFloat EntryRestoreUpdateDelegate;
		EntryRestoreUpdateDelegate.BindUFunction(this, FName("HandleEntryRestoreUpdate"));
		EntryRestoreTimeline.AddInterpFloat(EntryRestoreCurve, EntryRestoreUpdateDelegate);

		const float PlayRate = 1.0f / FMath::Max(TotalEntryRestoreTime, 0.1f);
		EntryRestoreTimeline.SetPlayRate(PlayRate);
		EntryRestoreTimeline.ReverseFromEnd();

		SetActorTickEnabled(true);
	}

	if (EntryMontagesToPlay.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, EntryMontagesToPlay.Num() - 1);
		if (UAnimMontage* SelectedMontage = EntryMontagesToPlay[RandomIndex])
		{
			PlayAnimMontage(SelectedMontage);
		}
	}
}

void ASL_EnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (DissolveTimeline.IsPlaying())
	{
		DissolveTimeline.TickTimeline(DeltaTime);
	}
	
	if (EntryRestoreTimeline.IsPlaying())
	{
		EntryRestoreTimeline.TickTimeline(DeltaTime);
	}
}

void ASL_EnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitEnemyStartupData();
}

#if WITH_EDITOR
void ASL_EnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionBoxAttachBoneName))
	{
		LeftHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionBoxAttachBoneName);
	}
	
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionBoxAttachBoneName))
	{
		RightHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionBoxAttachBoneName);
	}
}
#endif

void ASL_EnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (USL_FunctionLibrary::IsTargetPawnHostile(this, HitPawn))
		{
			EnemyCombatComponent->OnHitTargetActor(HitPawn);
		}
	}
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

	if (EnemyUIComponent)
	{
		EnemyUIComponent->RemoveEnemyDrawnWidgetsIfAny();
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

TObjectPtr<USL_EnemyUIComponent> ASL_EnemyCharacter::GetEnemyUIComponent() const
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
	
	bool bAbilityActivated = false;
	if (CharacterAbilitySystemComponent)
	{
		bAbilityActivated = CharacterAbilitySystemComponent->TryActivateAbilityByTag(SL_GameplayTags::Enemy_Ability_SpawnStone);
	}
	
	if (!bAbilityActivated)
	{
		SetLifeSpan(0.5f);
	}
}

void ASL_EnemyCharacter::HandleEntryRestoreUpdate(float Value)
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