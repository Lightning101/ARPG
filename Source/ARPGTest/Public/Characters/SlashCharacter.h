// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USlashOverlay;
enum class AttributeUpdateProperties : uint8;




UCLASS() class ARPGTEST_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASlashCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE ECharacterActionState GetActionState() const { return ActionState; };
	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; };

	void SetOverlappingItem(AItem* Item) override;
	void AddSouls(ASoul* Soul) override;
	void AddGold(ATreasure* Treasure);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeEnhancedInputMapping(APlayerController* PlayerController);

	void InitializeSlashOverlay(APlayerController* PlayerController);

	//  Input Controls Start //
	void Input_Move(const FInputActionValue& Value);
	bool CanMove();
	void Input_Look(const FInputActionValue& Value);
	void Input_Pickup(const FInputActionValue& Value);
	void Input_Attack(const FInputActionValue& Value);
	void Input_AttackHeavy(const FInputActionValue& Value);
	void Input_Dodge(const FInputActionValue& Value);

	bool CanDodge();

	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputMappingContext* DefaultInputMapping;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction* PickupAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction* AttackHeavyAction;
	UPROPERTY(EditAnywhere, Category = "Input Mappings")
	UInputAction* DodgeAction;

	//  Input Controls End //



	//  Animation Montages Start //
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* EquipMontage;
		UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* DodgeMontage;
	//  Animation Montages End //

	//  Animation Play Montages  Start //

	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;

	//  Animation Play Montages  End//
	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void FinishedEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	virtual void Attack(FName Section) override;
	bool CanAttack();
	void GetHit_Implementation(AActor* InitiatingActor, const FVector& ImpactPoint) override;

	virtual void Die() override;


private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;
	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	ECharacterWeaponEquipedState WeaponEquipedState = ECharacterWeaponEquipedState::ECWES_Unquipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterActionState ActionState = ECharacterActionState::ECAS_Unoccupied;

	UPROPERTY()
	USlashOverlay* SlashOverlay;
	void UpdateHUDAttribute(AttributeUpdateProperties AttributeProperty);

	UPROPERTY(EditAnywhere, Category="Combat")
	float DodgeStaminaCost = 10.f;

public:
	FORCEINLINE ECharacterWeaponEquipedState GetWeaponEquipedState()const { return WeaponEquipedState; }
};
