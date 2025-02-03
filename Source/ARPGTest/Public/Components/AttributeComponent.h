// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARPGTEST_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RegenStamina(float DeltaTime);

	void AddGold(int32 NumberOfGold);
	void AddSouls(int32 NumberOfSouls);
	void ReduceStamina(float StaminaCost);
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE int32 GetStamina() const { return Stamina; }


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;


	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold = 0;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls = 0;

public:
	void ReceiveDamage(float Damage);

	FORCEINLINE float GetPercentageHealth() const { return Health / MaxHealth; };
	FORCEINLINE float GetPercentageStamina() const { return Stamina / MaxStamina; };

	bool IsAlive() const;
};
