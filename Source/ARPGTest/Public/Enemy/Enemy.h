// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class USoundBase;
class UAttributeComponent;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class ARPGTEST_API AEnemy : public ABaseCharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector &ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.f;

protected:
	virtual void BeginPlay() override;

	//  Animation Montages Start //
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage *HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage *DeathMontage;

	void PlayMontage(UAnimMontage *Montage, const FName &SectionName) const;
	void Die();

	//  Animation Montages End //
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase *HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem *HitParticles;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	bool IsInTargetRange(AActor *Target, float Radius);

	void MoveToTarget(AActor *Target,float AcceptanceRadius = 15.f);

	AActor *SelectNewPatrolTarget();
	void CheckPatrolTarget();
	void CheckCombatTarget();

private:
	void DirectionalHitReact(const FVector &ImpactPoint);

	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent *Attributes;
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent *HealthBarWidget;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent *PawnSensing;

	UFUNCTION()
	void OnPawnSeen(APawn *Pawn);

	UPROPERTY()
	AActor *CombatTarget;

	/**
	 * Navigation
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


	EEnemyState EnemyState = EEnemyState::EES_Patroling;
};
