// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
    if (SlashCharacter)
    {
        MovementComponent = SlashCharacter->GetCharacterMovement();
    }
}

void USlashAnimInstance::NativeUpdateAnimation(float Deltatime)
{
    Super::NativeUpdateAnimation(Deltatime);

    if (MovementComponent)
    {
        GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
        IsFalling = MovementComponent->IsFalling();

        CharacterWeaponEquipedState = SlashCharacter->GetWeaponEquipedState();
    }
}
