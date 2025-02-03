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
#include "Components/StaticMeshComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Components/AttributeComponent.h"
#include "Items/Treasure/Treasure.h"
#include "Items/Soul.h"

enum class AttributeUpdateProperties : uint8 {
	AUP_Health,
	AUP_Stamina,
	AUP_Gold,
	AUP_Souls
};

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

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && Soul) {
		Attributes->AddSouls(Soul->GetSouls());
		UpdateHUDAttribute(AttributeUpdateProperties::AUP_Souls);
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && Treasure) {
		Attributes->AddGold(Treasure->GetGold());
		UpdateHUDAttribute(AttributeUpdateProperties::AUP_Gold);
	}
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		InitializeEnhancedInputMapping(PlayerController);
		InitializeSlashOverlay(PlayerController);
	}
}

void ASlashCharacter::InitializeEnhancedInputMapping(APlayerController* PlayerController)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (DefaultInputMapping)
		{
			Subsystem->AddMappingContext(DefaultInputMapping, 0);
		}
	}
}

void ASlashCharacter::InitializeSlashOverlay(APlayerController* PlayerController)
{
	if (ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD())) {
		SlashOverlay = SlashHUD->GetSlashOverlay();
		if (SlashOverlay && Attributes) {
			SlashOverlay->SetHealthBarPercent(Attributes->GetPercentageHealth());
		}
	}
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Attributes) {
		Attributes->RegenStamina(DeltaTime);
		UpdateHUDAttribute(AttributeUpdateProperties::AUP_Stamina);
	}
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Pickup);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Attack);
	EnhancedInputComponent->BindAction(AttackHeavyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_AttackHeavy);
	EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Input_Dodge);
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	UpdateHUDAttribute(AttributeUpdateProperties::AUP_Health);
	return DamageAmount;
}

void ASlashCharacter::UpdateHUDAttribute(AttributeUpdateProperties AttributeProperty)
{
	if (SlashOverlay && Attributes) {
		switch (AttributeProperty)
		{
		case AttributeUpdateProperties::AUP_Health:
			SlashOverlay->SetHealthBarPercent(Attributes->GetPercentageHealth());
			break;
			case AttributeUpdateProperties::AUP_Stamina:
				SlashOverlay->SetStaminaBarPercent(Attributes->GetPercentageStamina());
				break;
		case AttributeUpdateProperties::AUP_Gold:
			SlashOverlay->SetGold(Attributes->GetGold());
			break;
		case AttributeUpdateProperties::AUP_Souls:
			SlashOverlay->SetSouls(Attributes->GetSouls());
			break;
		default:
			break;
		}

	}
}

void ASlashCharacter::Input_Move(const FInputActionValue& Value)
{
	if (!CanMove())return;
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
bool ASlashCharacter::CanMove()
{
	return ActionState != ECharacterActionState::ECAS_Dead &&
		ActionState != ECharacterActionState::ECAS_Dodging;
}
void ASlashCharacter::Input_Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
}

void ASlashCharacter::Input_Pickup(const FInputActionValue& Value)
{

	if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
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
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
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

void ASlashCharacter::Input_Attack(const FInputActionValue& Value)
{

	Attack(FName("Attack1"));
}

void ASlashCharacter::Input_AttackHeavy(const FInputActionValue& Value)
{
	Attack(FName("Attack2"));
}

void ASlashCharacter::Input_Dodge(const FInputActionValue& Value)
{
	if (!CanDodge()) return;
	ActionState = ECharacterActionState::ECAS_Dodging;
	if (DodgeMontage) {
		Attributes->ReduceStamina(DodgeStaminaCost);
		PlayMontage(DodgeMontage, FName());
		UpdateHUDAttribute(AttributeUpdateProperties::AUP_Stamina);
	}
}

bool ASlashCharacter::CanDodge()
{
	return ActionState == ECharacterActionState::ECAS_Unoccupied &&
		Attributes &&
		Attributes->GetStamina() > DodgeStaminaCost;
}

void ASlashCharacter::AttackEnd()
{

	ActionState = ECharacterActionState::ECAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
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

void ASlashCharacter::HitReactEnd()
{
	ActionState = ECharacterActionState::ECAS_Unoccupied;
}

void ASlashCharacter::Attack(FName Section)
{
	//Super::Attack(Section);
	//if (CombatTarget == nullptr) return;
	if (CanAttack())
	{
		PlayMontage(AttackMontage, Section);
		ActionState = ECharacterActionState::ECAS_Attacking;
	}
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == ECharacterActionState::ECAS_Unoccupied &&
		WeaponEquipedState != ECharacterWeaponEquipedState::ECWES_Unquipped;
}

void ASlashCharacter::GetHit_Implementation(const AActor* InitiatingActor, const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(InitiatingActor, ImpactPoint);
	if (IsAlive())
		ActionState = ECharacterActionState::ECAS_HitReaction;
}

void ASlashCharacter::Die()
{
	Super::Die();
	ActionState = ECharacterActionState::ECAS_Dead;
}
