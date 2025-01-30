// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "ARPGTest/DebugShortcuts.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"

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
	SetHealthBarVisibility(false);

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
	if (EnemyState == EEnemyState::EES_Dead)
		return;

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
	bool NotInCombatRange = !IsInTargetRange(CombatTarget, CombatRadius);
	float ModifiedAttackRange = AttackRadius + (GetCapsuleComponent()->GetScaledCapsuleRadius() * 2);

	bool NotChasingAndNotInAttackRange = EnemyState != EEnemyState::EES_Chasing &&
										 !IsInTargetRange(CombatTarget, ModifiedAttackRange);

	bool NotAttackingAndInAttackRange = EnemyState != EEnemyState::EES_Attacking &&
										IsInTargetRange(
											CombatTarget, ModifiedAttackRange);

	if (NotInCombatRange)
	{
		// Outside combat radius
		ClearTimer(AttackTimer);
		LoseInterest();
		if (EnemyState != EEnemyState::EES_Engaged)
			StartPatrolling();
	}
	else if (NotChasingAndNotInAttackRange)
	{
		ClearTimer(AttackTimer);
		if (EnemyState != EEnemyState::EES_Engaged)
			ChaseTarget();
	}
	else if (NotAttackingAndInAttackRange)
	{
		if (EnemyState != EEnemyState::EES_Engaged)
		{
			EnemyState = EEnemyState::EES_Attacking;
			StartAttackTimer();
		}
	}
}

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint)
{
	SetHealthBarVisibility(true);
	if (IsAlive())
		DirectionalHitReact(ImpactPoint);
	else
		Die();

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void AEnemy::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetHealthBarVisibility(false);
	ClearTimer(AttackTimer);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	SetLifeSpan(DeathLifeSpan);
	EnemyState = EEnemyState::EES_Dead;
	if (DeathMontage && DeathMontage->GetNumSections() > 0)
	{
		int32 SectionIndex = FMath::RandRange(1, DeathMontage->GetNumSections());
		DeathPose = static_cast<EDeathPose>(SectionIndex - 1);
		FString SectionName = FString::Printf(TEXT("Death%d"), SectionIndex);
		PlayMontage(DeathMontage, FName(*SectionName));
	}
}

void AEnemy::Attack(FName Section)
{
	if (AttackMontage && AttackMontage->GetNumSections() > 0)
	{
		EnemyState = EEnemyState::EES_Engaged;
		int32 SectionIndex = FMath::RandRange(1, AttackMontage->GetNumSections());
		FString SectionName = FString::Printf(TEXT("Attack%d"), SectionIndex);
		PlayMontage(AttackMontage, FName(*SectionName));
	}
}
void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}
void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (HealthBarWidget)
		HealthBarWidget->SetHealthPercent(Attributes->GetPercentageHealth());
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
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	ChaseTarget();
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	Super::Destroy();
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}
void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::SetHealthBarVisibility(bool Visibility)
{
	if (HealthBarWidget)
		HealthBarWidget->SetVisibility(Visibility);
}

void AEnemy::LoseInterest()
{
	SetHealthBarVisibility(false);
	CombatTarget = nullptr;
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patroling;
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget, AttackRadius);
}

void AEnemy::ClearTimer(FTimerHandle TimerHandle)
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void AEnemy::StartAttackTimer()
{
	const float AttackTime = FMath::RandRange(AttackTimeMin, AttackTimeMax);
	FTimerDelegate ExplosionDelegate = FTimerDelegate::CreateLambda([this]()
																	{ Attack(FName()); });
	GetWorldTimerManager().SetTimer(AttackTimer, ExplosionDelegate, AttackTime, false);
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
	const bool bShouldChaseTarget =
		EnemyState == EEnemyState::EES_Patroling &&
		Pawn->ActorHasTag(FName("SlashCharacter"));

	if (bShouldChaseTarget)
	{
		CombatTarget = Pawn;
		ClearTimer(PatrolTimer);
		ChaseTarget();
	}
}