// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Birds.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext ;
class UInputAction ;


UCLASS()
class ARPGTEST_API ABirds : public APawn
{
	GENERATED_BODY()

public:
	ABirds();
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);


	UPROPERTY(EditAnywhere, Category="Input Mappings")
	UInputMappingContext*  DefaultInputMapping;
	UPROPERTY(EditAnywhere, Category="Input Mappings")
	UInputAction*  MoveAction;
	UPROPERTY(EditAnywhere, Category="Input Mappings")
	UInputAction*  LookAction;

private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsualComponent;
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

};
