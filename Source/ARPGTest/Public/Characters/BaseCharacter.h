// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
UCLASS()
class ARPGTEST_API ABaseCharacter : public ACharacter
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
	virtual void Attack(FName Section){};
	void PlayMontage(UAnimMontage *Montage, const FName &Selection);

private:
};
