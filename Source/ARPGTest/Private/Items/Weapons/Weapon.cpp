// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());

    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
    WeaponBox->SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AWeapon::BeginPlay()
{

    Super::BeginPlay();
    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);

    if (ItemState == EItemState::EIS_Hovering)
    {
        PickupEffect->Activate();
    }
    else
    {
        PickupEffect->Deactivate();
    }
}

void AWeapon::Equip(USceneComponent *InParent, FName InSocketName, AActor *NewOwner, APawn *NewInsitgator)
{
    ItemState = EItemState::EIS_Equiped;
    SetOwner(NewOwner);
    SetInstigator(NewInsitgator);
    AttachMeshToSocket(InParent, FName("RightHandSocket"));
    SetSphereCollisionEnabled(false);
    SetPickupEffectEnabled(false);
    PlayEquipSound();
}

void AWeapon::SetPickupEffectEnabled(bool bEnabled)
{
    if (PickupEffect) {
        if (bEnabled)
            PickupEffect->Activate();
        else
        PickupEffect->Deactivate();
    }
}

void AWeapon::SetSphereCollisionEnabled(bool bEnabled)
{
	if (Sphere)
	{
		if (bEnabled)
			Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		else
			Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::BoxTrace(FHitResult& OutBoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
    if (Owner)
        ActorsToIgnore.Add(Owner);


	for (AActor* Actor : IgnoreActors)
		ActorsToIgnore.AddUnique(Actor);

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(5.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutBoxHit,
		true);
	IgnoreActors.AddUnique(OutBoxHit.GetActor());

}

void AWeapon::PlayEquipSound()
{
    if (EquipSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
    }
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    // Ignore other enemies
    if (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy")))return;

    FHitResult HitResult;
    BoxTrace(HitResult);
    if (HitResult.GetActor())
    {
        // Ignore other enemies
        if (GetOwner()->ActorHasTag(TEXT("Enemy")) && HitResult.GetActor()->ActorHasTag(TEXT("Enemy")))return;
        

        UGameplayStatics::ApplyDamage(
            HitResult.GetActor(),
            Damage, GetInstigator()->GetController(),
            this,
            UDamageType::StaticClass());

        ExecuteBoxHit(HitResult);
        CreateFields(HitResult.ImpactPoint);
    }
}

void AWeapon::ExecuteBoxHit(FHitResult& HitResult)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor()))
	{
		HitInterface->Execute_GetHit(HitResult.GetActor(),GetOwner(), HitResult.ImpactPoint);
	}
}
