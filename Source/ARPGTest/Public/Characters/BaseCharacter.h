// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
class UAttributeComponent;
class USoundBase;
UCLASS()
class ARPGTEST_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

	// ====== Unreal Functions Start ======
public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// ====== Unreal Functions End ======

	// ====== Custom Functions Start ======

public:
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	UPROPERTY(VisibleAnywhere)
	AWeapon *EquippedWeapon;
	virtual void Attack(FName Section) {};
	virtual void Die();
	bool IsAlive();

	void PlayMontage(UAnimMontage *Montage, const FName &Selection) const;
	virtual void DirectionalHitReact(const FVector &ImpactPoint);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	//  Animation Montages Start //
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage *AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage *HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage *DeathMontage;
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);

	//  Animation Montages End //

	// Components Start //
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent *Attributes;
	// Components End //



private:
	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem *HitParticles;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase *HitSound;
};
