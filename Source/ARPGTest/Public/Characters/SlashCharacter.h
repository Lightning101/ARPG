// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Characters/CharacterTypes.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UGroomComponent;
class AItem;
class UAnimMontage;




UCLASS() class ARPGTEST_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASlashCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//  Input Controls Start //
	void Move(const FInputActionValue &Value);
	void Look(const FInputActionValue &Value);
	void Pickup(const FInputActionValue &Value);
	void Attack(const FInputActionValue &Value);
	void AttackHeavy(const FInputActionValue &Value);

	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputMappingContext *DefaultInputMapping;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction *MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction *LookAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction *JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction *PickupAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction *AttackAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction *AttackHeavyAction;

	//  Input Controls End //



	//  Animation Montages Start //
	UPROPERTY(EditDefaultsOnly, Category="Montage")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category="Montage")
	UAnimMontage* EquipMontage;
	//  Animation Montages End //

	//  Animation Play Montages  Start //

	void PlayAttackMontage(const FName& Selection);

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	//  Animation Play Montages  End//
	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void FinishedEquipping();



private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent *SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent *ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent *Hair;
	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent *Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem *OverlappingItem;

	ECharacterWeaponEquipedState WeaponEquipedState =  ECharacterWeaponEquipedState::ECWES_Unquipped;
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	ECharacterActionState ActionState =  ECharacterActionState::ECAS_Unoccupied;

public:
	FORCEINLINE void SetOverlappingItem(AItem *Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterWeaponEquipedState GetWeaponEquipedState()const{return WeaponEquipedState;}
};
