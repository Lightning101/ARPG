// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Soul.generated.h"

/**
 *
 */
UCLASS()
class ARPGTEST_API ASoul : public AItem
{
	GENERATED_BODY()

public:
	FORCEINLINE int32 GetSouls()const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Souls Properties")
	int32 Souls;



};
