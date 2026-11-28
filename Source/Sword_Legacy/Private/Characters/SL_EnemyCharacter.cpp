#include "Characters/SL_EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/SL_EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartupData/SL_DataAsset_StartupData_Enemy.h"
#include "Items/Weapons/SL_WeaponBase.h"
#include "Components/MeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/UI/SL_EnemyUIComponent.h"
#include "UI/Widgets/SL_WidgetBase.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_DeathBase.h"
#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_ParryRebound.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "GameModes/SL_GameModeBase.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

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

	if (!IsNetStartupActor() && !HasAnyFlags(RF_WasLoaded))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASL_GameModeBase* GameMode = World->GetAuthGameMode<ASL_GameModeBase>())
			{
				GameMode->RegisterSummonedEnemy(this);
			}
		}
	}

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

		FOnTimelineEvent EntryRestoreFinishedDelegate;
		EntryRestoreFinishedDelegate.BindUFunction(this, FName("HandleEntryRestoreFinished"));
		EntryRestoreTimeline.SetTimelineFinishedFunc(EntryRestoreFinishedDelegate);

		bEntryRestoreFinished = false;
		CurrentEntryDissolveAmount = 1.f;
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

void ASL_EnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (StartupDataStreamableHandle.IsValid())
	{
		StartupDataStreamableHandle->CancelHandle();
		StartupDataStreamableHandle.Reset();
	}

	Super::EndPlay(EndPlayReason);
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
	if (UWorld* World = GetWorld())
	{
		if (ASL_GameModeBase* GameMode = World->GetAuthGameMode<ASL_GameModeBase>())
		{
			GameMode->NotifyEnemyRosterDeath(this);
		}
	}

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

	if (StartupDataStreamableHandle.IsValid())
	{
		StartupDataStreamableHandle->CancelHandle();
	}

	StartupDataStreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartupData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[WeakThis = TWeakObjectPtr<ASL_EnemyCharacter>(this), SoftStartupData = CharacterStartupData]()
			{
				ASL_EnemyCharacter* StrongThis = WeakThis.Get();
				
				if (!IsValid(StrongThis) || !IsValid(StrongThis->CharacterAbilitySystemComponent)) return;

				if (USL_DataAsset_StartupData_Base* LoadedData = SoftStartupData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(
						StrongThis->CharacterAbilitySystemComponent,
						USL_FunctionLibrary::GetCurrentGameDifficultyAbilityLevel()
					);
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
		SetLifeSpan(DestroyDelayWhenNoStoneSpawn);
	}
}

void ASL_EnemyCharacter::HandleEntryRestoreUpdate(float Value)
{
	CurrentEntryDissolveAmount = Value;

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

void ASL_EnemyCharacter::HandleEntryRestoreFinished()
{
	bEntryRestoreFinished = true;
	HandleEntryRestoreUpdate(0.f);
}

bool ASL_EnemyCharacter::WantsToBlockIncomingAttack(const AActor* Attacker) const
{
	if (!Attacker || BlockChance <= 0.f) return false;

	if (USL_FunctionLibrary::NativeDoesActorHaveTag(const_cast<ASL_EnemyCharacter*>(this), SL_GameplayTags::Shared_Status_Death)) return false;

	if (FVector::Dist2D(GetActorLocation(), Attacker->GetActorLocation()) > DefenseDistance) return false;

	return FMath::FRand() <= BlockChance;
}

bool ASL_EnemyCharacter::RespondToIncomingAttack(const AActor* Attacker)
{
	if (!WantsToBlockIncomingAttack(Attacker)) return false;

	USL_AbilitySystemComponent* AbilitySystemComponent = GetCharacterAbilitySystemComponent();
	if (!AbilitySystemComponent) return false;

	FGameplayTagContainer AttackTags;
	AttackTags.AddTag(SL_GameplayTags::Enemy_Ability_Melee);
	AttackTags.AddTag(SL_GameplayTags::Enemy_Ability_HeavyMelee);
	AttackTags.AddTag(SL_GameplayTags::Enemy_Ability_Combo);
	AbilitySystemComponent->CancelAbilities(&AttackTags);

	if (!AbilitySystemComponent->TryActivateAbilityByTag(SL_GameplayTags::Enemy_Ability_Block)) return false;

	USL_FunctionLibrary::AddGameplayTagToActorIfNone(this, SL_GameplayTags::Enemy_Status_IncomingAttack);

	return true;
}

void ASL_EnemyCharacter::ForceParryRebound(AActor* InstigatorActor)
{
	USL_AbilitySystemComponent* AbilitySystemComponent = GetCharacterAbilitySystemComponent();

	if (!AbilitySystemComponent) return;

	GetWorldTimerManager().ClearTimer(ParryReboundLockTimer);

	USL_FunctionLibrary::AddGameplayTagToActorIfNone(this, SL_GameplayTags::Enemy_Status_Rebounding);
	USL_FunctionLibrary::AddGameplayTagToActorIfNone(this, SL_GameplayTags::Enemy_Status_UnderAttack);

	if (InstigatorActor)
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), InstigatorActor->GetActorLocation());

		SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));
	}

	FGameplayAbilitySpecHandle ReboundHandle;
	UAnimMontage* ReboundMontage = nullptr;

	for (int32 Pass = 0; Pass < 2; ++Pass)
	{
		TArray<FGameplayAbilitySpecHandle> HandlesToCancel;

		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (!Spec.Ability) continue;

			if (Spec.Ability->GetClass()->IsChildOf(USL_GA_Enemy_ParryRebound::StaticClass()))
			{
				ReboundHandle = Spec.Handle;

				if (const USL_GA_Enemy_ParryRebound* ReboundAbility = Cast<USL_GA_Enemy_ParryRebound>(Spec.Ability))
				{
					ReboundMontage = ReboundAbility->GetReboundMontage();
				}

				continue;
			}

			if (Spec.Ability->GetClass()->IsChildOf(USL_GA_Enemy_DeathBase::StaticClass())) continue;

			if (Spec.IsActive())
			{
				HandlesToCancel.Add(Spec.Handle);
			}
		}

		if (HandlesToCancel.IsEmpty()) break;

		for (const FGameplayAbilitySpecHandle& Handle : HandlesToCancel)
		{
			AbilitySystemComponent->CancelAbilityHandle(Handle);
		}
	}

	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.f);
			AnimInstance->Montage_Stop(0.f);
		}
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}
	
	if (ReboundHandle.IsValid() && AbilitySystemComponent->TryActivateAbility(ReboundHandle)) return;

	float ReboundLength = 0.2f;

	if (ReboundMontage)
	{
		if (USkeletalMeshComponent* CharacterMesh = GetMesh())
		{
			if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
			{
				const float PlayedLength = AnimInstance->Montage_Play(ReboundMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
				
				if (PlayedLength > 0.f)
				{
					ReboundLength = PlayedLength;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		ParryReboundLockTimer,
		this,
		&ASL_EnemyCharacter::ReleaseParryReboundLock,
		ReboundLength,
		false
	);
}

void ASL_EnemyCharacter::ReleaseParryReboundLock()
{
	USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(this, SL_GameplayTags::Enemy_Status_Rebounding);
	USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(this, SL_GameplayTags::Enemy_Status_UnderAttack);
}

float ASL_EnemyCharacter::GetSpawnedWeaponDissolveAmount() const
{
	if (!EntryRestoreCurve || bEntryRestoreFinished)
	{
		return 0.f;
	}

	if (EntryRestoreTimeline.IsPlaying())
	{
		return CurrentEntryDissolveAmount;
	}

	return 1.f;
}