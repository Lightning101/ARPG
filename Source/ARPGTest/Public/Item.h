// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UNiagaraComponent;

enum class EItemState : uint8
{
	EIS_Hovering UMETA(DisplayName = "Hovering"),
	EIS_Unquipped UMETA(DisplayName = "Unquipped"),
	EIS_Equiped UMETA(DisplayName = "Equipped"),
};

UCLASS()
class ARPGTEST_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AttachMeshToSocket(USceneComponent *InParent, const FName &InSocketName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	float TransformedSin(float value) const;
	UFUNCTION(BlueprintPure)
	float TransformedCos(float Value) const;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	virtual void OnSphereOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *StaticMesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	USphereComponent *Sphere;

	UPROPERTY(EditAnywhere, Category = "Item Presentation")
	UNiagaraComponent *PickupEffect;

private:
	float Runtime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Item Presentation")
	float Amplitude = 0.25f;
	UPROPERTY(EditAnywhere, Category = "Item Presentation")
	float TimeContant = 5.f;
};
