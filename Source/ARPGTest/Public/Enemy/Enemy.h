// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;
class AWeapon;

UCLASS()
class ARPGTEST_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector &ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.f;

protected:
	virtual void BeginPlay() override;

	//  Animation Montages Start //
	virtual void Die() override;

	virtual void Attack(FName Section) override;

	virtual void HandleDamage(float DamageAmmount) override;

	//  Animation Montages End //
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patroling;

	bool IsInTargetRange(AActor *Target, float Radius);

	void MoveToTarget(AActor *Target, float AcceptanceRadius = 15.f);

	AActor *SelectNewPatrolTarget();
	void CheckPatrolTarget();
	void CheckCombatTarget();

private:
	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent *HealthBarWidget;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent *PawnSensing;

	UFUNCTION()
	void OnPawnSeen(APawn *Pawn);

	UPROPERTY()
	AActor *CombatTarget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	/**
	 * Navigation/AI Start
	 */

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor *PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor *> PatrolTargets;

	UPROPERTY()
	AAIController *EnemyController;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;
	UPROPERTY(EditAnywhere)
	float PatrolDelayMin = 3.f;
	UPROPERTY(EditAnywhere)
	float PatrolDelayMax = 5.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	/**
	 * Navigation/AI End
	 */

	/**
	 * AI Behavior Start
	 */

	void SetHealthBarVisibility(bool Visibility);
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	void ClearTimer(FTimerHandle TimerHandle);

	// Combat Start
	FTimerHandle AttackTimer;
	void StartAttackTimer();

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackTimeMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackTimeMax = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrolSpeed = 125.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	// Combat End

	/**
	 * AI Behavior End
	 */
};
