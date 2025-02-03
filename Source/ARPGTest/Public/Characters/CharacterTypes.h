
#pragma once

UENUM(BlueprintType)
enum class ECharacterWeaponEquipedState : uint8
{
	ECWES_Unquipped UMETA(DisplayName = "Unquipped"),
	ECWES_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One Handed Weapon"),
	ECWES_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two Handed Weapon")

};

UENUM(BlueprintType)
enum class ECharacterActionState : uint8
{
	EES_NoState UMETA(DisplayName = "No State"),
	ECAS_Dead UMETA(DisplayName = "Dead"),
	ECAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECAS_HitReaction UMETA(DisplayName = "HitReaction"),
	ECAS_Dodging UMETA(DisplayName = "Dodging"),
	ECAS_Attacking UMETA(DisplayName = "Attacking"),
	ECAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")

};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),
	EDP_Death6 UMETA(DisplayName = "Death6"),

	// Check when converting from int32
	// To ensure within enum classes 
	EDP_MAX UMETA(DisplayName = "MAX"),

};


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "No State"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patroling UMETA(DisplayName = "Patroling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged")
};