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

	/**
	 * Engine Functions
	 */
public:
	// Sets default values for this character's properties
	AEnemy();
	/** <Actor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </Actor> */

protected:
	virtual void BeginPlay() override;

	/**
	 * Custom Functions
	 */
public:
	//  Combat  Start

	/** <IHitInterface */
	virtual void GetHit_Implementation(const AActor* InitiatingActor, const FVector& ImpactPoint) override;
	/** </IHitInterface */

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRadius = 150.f;
	//  Combat  End

protected:
	// ABaseCharacter Overrides Start //
	virtual void Die() override;
	virtual void Attack(FName Section) override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmmount) override;


	// ABaseCharacter Overrides End //

	// Enemy States Start //
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patroling;

	// Enemy States End //

private:
	/** Components Start */

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	/** Components End */

	/**
	 * AI Behavior Start
	 */

	void CheckPatrolTarget();
	void CheckCombatTarget();

	void PatrolTimerFinished();
	void SetHealthBarVisibility(bool Visibility);
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	void ClearTimer(FTimerHandle TimerHandle);

	bool IsInTargetRange(AActor* Target, float Radius);

	void MoveToTarget(AActor* Target, float AcceptanceRadius = 15.f);

	AActor* SelectNewPatrolTarget();

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double PatrolRadius = 200.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrolDelayMin = 3.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrolDelayMax = 5.f;

	FTimerHandle PatrolTimer;

	// Combat Start
	void StartAttackTimer();

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);


	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackTimeMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackTimeMax = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrolSpeed = 125.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;
	// Combat End

	/**
	 * AI Behavior End
	 */
};
