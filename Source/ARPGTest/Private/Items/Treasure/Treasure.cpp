// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Treasure/Treasure.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
    if (PickupInterface && PickupSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this,PickupSound,GetActorLocation());
        PickupInterface->AddGold(this);
        Destroy();
        
    }

}
