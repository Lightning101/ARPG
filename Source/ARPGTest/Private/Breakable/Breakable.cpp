// Fill out your copyright notice in the Description page of Project Settings.

#include "Breakable/Breakable.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure/Treasure.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABreakable::ABreakable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);

	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void ABreakable::BeginPlay()
{
	Super::BeginPlay();
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakable::OnBreak);
}

void ABreakable::OnBreak(const FChaosBreakEvent &BreakEvent)
{
	if (BrokenLevel < 1)
	{
		GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GeometryCollection->OnActorEnableCollisionChanged();
		UE_LOG(LogTemp, Warning, TEXT("Broken Component"));
		UWorld *World = GetWorld();
		int32 TreasureClassSize = TreasureClassArray.Num();
		if (TreasureClassSize > 0 && World)
		{
			FVector DropSpawnLocation = GetActorLocation() + FVector(0.f, 0.f, DropSpawnHeight);
			int32 SpawnTreasureClassIndex = FMath::RandRange(0, TreasureClassSize - 1);
			

			if (TreasureClassArray[SpawnTreasureClassIndex])
			{
				// World->SpawnActor<ATreasure>(TreasureClassArray[SpawnTreasureClassIndex], DropSpawnLocation, GetActorRotation());
				FTransform SpawnTransform = FTransform(GetActorRotation(), DropSpawnLocation);
				ATreasure *Treasure = World->SpawnActorDeferred<ATreasure>(TreasureClassArray[SpawnTreasureClassIndex], SpawnTransform, nullptr, nullptr);
				float SpawnTreasureChance = FMath::FRand();
				if (SpawnTreasureChance > (1.f - Treasure->DropChance))
				{
					UGameplayStatics::FinishSpawningActor(Treasure, SpawnTransform);
				}
				else
				{
					Treasure->Destroy();
				}
			}
		}
		BrokenLevel++;
	}
}
// Called every frame
void ABreakable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakable::GetHit_Implementation(const FVector &ImpactPoint)
{
	// UWorld *World = GetWorld();
	// if (TreasureClass && World)
	// {
	// 	FVector DropSpawnLocation = GetActorLocation() + FVector(0.f, 0.f, DropSpawnHeight);
	// 	World->SpawnActor<ATreasure>(TreasureClass, DropSpawnLocation, GetActorRotation());
	// }
}
