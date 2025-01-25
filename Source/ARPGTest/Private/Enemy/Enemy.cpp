// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "ARPGTest/DebugShortcuts.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget)
		HealthBarWidget->SetVisibility(false);

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);
	}
	UWorld *World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon *Weapon = World->SpawnActor<AWeapon>(WeaponClass);
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = Weapon;
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EnemyState > EEnemyState::EES_Patroling)
		CheckCombatTarget();
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (IsInTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = SelectNewPatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(PatrolDelayMin, PatrolDelayMax));
	}
}

void AEnemy::CheckCombatTarget()
{
	if (!IsInTargetRange(CombatTarget, CombatRadius))
	{
		// Outside combat radius
		CombatTarget = nullptr;
		if (HealthBarWidget)
			HealthBarWidget->SetVisibility(false);
		EnemyState = EEnemyState::EES_Patroling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);
	}
	else if (EnemyState != EEnemyState::EES_Chasing && !IsInTargetRange(CombatTarget, AttackRadius))
	{
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget, AttackRadius);
	}
	else if (EnemyState != EEnemyState::EES_Attacking && IsInTargetRange(CombatTarget, AttackRadius + (GetCapsuleComponent()->GetScaledCapsuleRadius() * 2)))
	{
		EnemyState = EEnemyState::EES_Attacking;
		// !TODO Attack Montage
	}
}
// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint)
{
	if (HealthBarWidget)
		HealthBarWidget->SetVisibility(true);

	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
	if (HitParticles)
	{

		UGameplayStatics::SpawnEmitterAttached(HitParticles, GetMesh(), FName(""), ImpactPoint, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition);
	}
}

void AEnemy::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);

	if (HealthBarWidget)
		HealthBarWidget->SetVisibility(false);

	if (DeathMontage)
	{
		int32 SectionIndex = FMath::RandRange(1, DeathMontage->GetNumSections());
		DeathPose = EDeathPose((uint8)SectionIndex);
		FString SectionName = FString::Printf(TEXT("Death%d"), SectionIndex);
		PlayMontage(DeathMontage, FName(*SectionName));
	}
}

bool AEnemy::IsInTargetRange(AActor *Target, float Radius)
{
	if (Target == nullptr)
		return false;
	double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{

	if (Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetPercentageHealth());
	}
	CombatTarget = EventInstigator->GetPawn();
	EnemyState = EEnemyState::EES_Chasing;
	MoveToTarget(CombatTarget, AttackRadius);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	return DamageAmount;
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::MoveToTarget(AActor *Target, float AcceptanceRadius)
{
	if (!EnemyController || !Target)
	{
		EnemyController->StopMovement();
		return;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	EnemyController->MoveTo(MoveRequest);
	// FNavPathSharedPtr NavPath;
	// EnemyController->MoveTo(MoveRequest, &NavPath);

	// TArray<FNavPathPoint> &PathPoints = NavPath->GetPathPoints();

	// for (auto &Point : PathPoints)
	// {
	// 	const FVector &Location = Point.Location;

	// 	DRAW_SPHERE_TEMP_COLOR_SIZE(Location, FColor::Purple, 10.f, 10.f);
	// }
}

AActor *AEnemy::SelectNewPatrolTarget()
{
	int32 PatrolTargetsSize = PatrolTargets.Num();
	if (PatrolTargetsSize != 0)
	{
		int32 CurrentPatrolTargetIndex = PatrolTargets.Find(PatrolTarget);
		if (CurrentPatrolTargetIndex != INDEX_NONE)
			return PatrolTargets[(CurrentPatrolTargetIndex + 1) % PatrolTargetsSize];
	}
	return nullptr;
}

void AEnemy::OnPawnSeen(APawn *Pawn)
{

	if (EnemyState == EEnemyState::EES_Chasing)
		return;

	if (Pawn->ActorHasTag(FName("SlashCharacter")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = Pawn;
		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(Pawn, AttackRadius);
		}
	}
}