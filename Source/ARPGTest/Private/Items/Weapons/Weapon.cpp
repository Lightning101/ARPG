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

    SetOwner(NewOwner);
    SetInstigator(NewInsitgator);
    AttachMeshToSocket(InParent, FName("RightHandSocket"));
    ItemState = EItemState::EIS_Equiped;

    PickupEffect->Deactivate();
    if (Sphere)
    {
        Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (EquipSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
    }
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::OnSphereOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();
    const FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor *> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    FHitResult HitResult;

    for (AActor *Actor : IgnoreActors)
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
        EDrawDebugTrace::None,
        HitResult,
        true);

    if (HitResult.GetActor())
    {
        UGameplayStatics::ApplyDamage(
            HitResult.GetActor(),
            Damage, GetInstigator()->GetController(),
            this,
            UDamageType::StaticClass());

        if (IHitInterface *HitInterface = Cast<IHitInterface>(HitResult.GetActor()))
        {

            IgnoreActors.AddUnique(HitResult.GetActor());
            HitInterface->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint);
        }
        CreateFields(HitResult.ImpactPoint);
    }
}
