// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterTypes.h"
#include "SlashAnimInstance.generated.h"




class ASlashCharacter;
class UCharacterMovementComponent;


/**
 *
 */
UCLASS()
class ARPGTEST_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float Deltatime) override;

	UPROPERTY(BlueprintReadOnly)
	ASlashCharacter* SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsFalling;
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
	ECharacterWeaponEquipedState CharacterWeaponEquipedState;

	UPROPERTY(BlueprintReadOnly, Category = "Action State")
	ECharacterActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = "Character State")
	EDeathPose DeathPose;

};
