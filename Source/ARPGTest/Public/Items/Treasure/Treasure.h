// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class ARPGTEST_API ATreasure : public AItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float DropChance = 0.5f;


protected:

	virtual void OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase *PickupSound;

	UPROPERTY(EditAnywhere)
	int32 GoldValue = 5;
};
