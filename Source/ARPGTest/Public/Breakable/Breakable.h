// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Physics/Experimental/ChaosEventType.h"
#include "Breakable.generated.h"

class UGeometryCollectionComponent;
UCLASS()
class ARPGTEST_API ABreakable : public AActor, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABreakable();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(AActor* InitiatingActor, const FVector& ImpactPoint) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnBreak(const FChaosBreakEvent& BreakEvent);

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent *GeometryCollection;
	
	UPROPERTY(EditAnywhere)
	float DropSpawnHeight = 75.f;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf <class ATreasure>> TreasureClassArray;


	int32 BrokenLevel = 0;
};
