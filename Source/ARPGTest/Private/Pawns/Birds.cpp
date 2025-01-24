// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/Birds.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

ABirds::ABirds()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CapsualComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsualComponent"));
	SetRootComponent(CapsualComponent);

	StaticMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	StaticMesh->SetupAttachment(GetRootComponent());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	ViewCamera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ABirds::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController *PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if(DefaultInputMapping)
			{
				Subsystem->AddMappingContext(DefaultInputMapping, 0);
			}
			
		}
	}
}

void ABirds::Move(const FInputActionValue& Value)
{
	if (Controller)
	{
		FVector2D MoveVector = Value.Get<FVector2D>();
		AddMovementInput(GetActorForwardVector(), MoveVector.X);
		AddMovementInput(GetActorRightVector(), MoveVector.Y);
	}
}
void ABirds::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
}
// Called every frame
void ABirds::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABirds::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ABirds::Move);
	EnhancedInputComponent->BindAction(LookAction,ETriggerEvent::Triggered,this,&ABirds::Look);
}
