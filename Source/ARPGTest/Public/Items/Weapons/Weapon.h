// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
/**
 *
 */
UCLASS()
class ARPGTEST_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	UFUNCTION(BlueprintCallable)
	void Equip(USceneComponent *InParent, FName InSocketName, AActor* NewOwner, APawn* NewInsitgator);
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	void ExecuteBoxHit(FHitResult& HitResult);
	// Overlap Events End //

	// void OnBoxeOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)override;

	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	USoundBase *EquipSound;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	UBoxComponent *WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent *BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent *BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float Damage  = 20.f;

	void PlayEquipSound();
	void SetPickupEffectEnabled(bool bEnabled);
	void SetSphereCollisionEnabled(bool bEnabled);

	void BoxTrace(FHitResult& OutBoxHit);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	
};
