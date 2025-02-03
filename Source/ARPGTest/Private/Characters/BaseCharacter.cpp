// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::Attack(FName Section)
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead"))) {
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Die()
{
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayDeathMontage();
	DisableMeshCollision();
	Tags.Add(FName(TEXT("Dead")));
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(false);
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}
void ABaseCharacter::GetHit_Implementation(const AActor* InitiatingActor, const FVector& ImpactPoint)
{
	if (IsAlive() && InitiatingActor)
		DirectionalHitReact(InitiatingActor->GetActorLocation());
	else
		Die();

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopMontage(AttackMontage);

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}
void ABaseCharacter::PlayMontage(UAnimMontage *Montage, const FName &Selection) const
{
	UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(Selection, Montage);
	}
}

void ABaseCharacter::StopMontage(UAnimMontage* Montage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Stop(0.25f,Montage);

	}
}

void ABaseCharacter::DirectionalHitReact(const FVector &ImpactPoint)
{

	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	double Theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, ToHit)));

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}
	// DRAW_SPHERE_TEMP_COLOR_SIZE(ImpactPoint, FColor::Red, 10.f, 2.f);

	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Emerald, FString::Printf(TEXT("Theta value %f"), Theta));
	// }

	// UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetActorLocation(), GetActorLocation() + Forward * 100.f, 100.f, FColor::Green, 5.f);
	// UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ToHit * 100.f, 100.f, FColor::Red, 5.f);
	// UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 100.f, FColor::Blue, 5.f);

	FName Section = FName("React_Back");

	if (Theta >= -45.f and Theta <= 45.f)
	{
		Section = FName("React_Front");
	}
	else if (Theta >= -135 and Theta < -45.f)
	{
		Section = FName("React_Left");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("React_Right");
	}

	PlayMontage(HitReactMontage, Section);
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DodgeEnd()
{
}

FVector ABaseCharacter::UpdateTranslationWarping() const
{
	if (CombatTarget) {
		const FVector& CombatTargetLocation = CombatTarget->GetActorLocation();
		FVector Location = (GetActorLocation() - CombatTargetLocation).GetSafeNormal();
		Location *= MotionWarpingTranslationBuffer;
		return CombatTargetLocation + Location;
	}
	return FVector();
}

void ABaseCharacter::PlayDeathMontage()
{
	if (DeathMontage && DeathMontage->GetNumSections() > 0)
	{
		int32 SectionIndex = FMath::RandRange(1, DeathMontage->GetNumSections());
		DeathPose = static_cast<EDeathPose>(SectionIndex - 1);
		FString SectionName = FString::Printf(TEXT("Death%d"), SectionIndex);
		PlayMontage(DeathMontage, FName(*SectionName));
	}
}

FVector ABaseCharacter::UpdateRotationWarping() const
{
	if (CombatTarget) {
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::PlayHitSound(const FVector &ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector &ImpactPoint)
{
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAttached(HitParticles, GetMesh(), FName(""), ImpactPoint, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
		Attributes->ReceiveDamage(DamageAmount);
}
