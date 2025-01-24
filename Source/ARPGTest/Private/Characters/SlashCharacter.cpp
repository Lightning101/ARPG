// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	ViewCamera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("SlashCharacter"));
	if (APlayerController *PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultInputMapping)
			{
				Subsystem->AddMappingContext(DefaultInputMapping, 0);
			}
		}
	}
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Pickup);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Attack);
	EnhancedInputComponent->BindAction(AttackHeavyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_AttackHeavy);
}

void ASlashCharacter::Input_Move(const FInputActionValue &Value)
{
	if (ActionState != ECharacterActionState::ECAS_Unoccupied)
		return;
	if (Controller && ViewCamera)
	{
		FVector2D MoveVector = Value.Get<FVector2D>();
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);

		FVector PlayerForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector PlayerRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if (MoveVector.X != 0)
		{
			AddMovementInput(PlayerForward, MoveVector.X);
		}
		if (MoveVector.Y != 0)
		{
			AddMovementInput(PlayerRight, MoveVector.Y);
		}
	}
}
void ASlashCharacter::Input_Look(const FInputActionValue &Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
}

void ASlashCharacter::Input_Pickup(const FInputActionValue &Value)
{

	if (AWeapon *OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		WeaponEquipedState = ECharacterWeaponEquipedState::ECWES_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}
	else if (EquippedWeapon)
	{

		if (ActionState == ECharacterActionState::ECAS_Unoccupied)
		{
			UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && EquipMontage)
			{
				ActionState = ECharacterActionState::ECAS_EquippingWeapon;
				FName EquipAction = FName("Equip");
				ECharacterWeaponEquipedState EquipedState = ECharacterWeaponEquipedState::ECWES_EquippedOneHandedWeapon;

				if (WeaponEquipedState != ECharacterWeaponEquipedState::ECWES_Unquipped)
				{
					EquipAction = FName("Unequip");
					EquipedState = ECharacterWeaponEquipedState::ECWES_Unquipped;
				}
				AnimInstance->Montage_Play(EquipMontage);
				AnimInstance->Montage_JumpToSection(EquipAction, EquipMontage);
				WeaponEquipedState = EquipedState;
			}
		}
	}
}

void ASlashCharacter::Input_Attack(const FInputActionValue &Value)
{
	Attack(FName("Attack1"));
}

void ASlashCharacter::Input_AttackHeavy(const FInputActionValue &Value)
{
	Attack(FName("Attack2"));
}

void ASlashCharacter::AttackEnd()
{

	ActionState = ECharacterActionState::ECAS_Unoccupied;
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}
void ASlashCharacter::FinishedEquipping()
{
	if (EquippedWeapon)
	{
		ActionState = ECharacterActionState::ECAS_Unoccupied;
	}
}

void ASlashCharacter::Attack(FName Section)
{
	if (ActionState == ECharacterActionState::ECAS_Unoccupied &&
		WeaponEquipedState != ECharacterWeaponEquipedState::ECWES_Unquipped)
	{
		PlayMontage(AttackMontage,Section);
		ActionState = ECharacterActionState::ECAS_Attacking;
	}
}
