#pragma once

#include "BaseWeaponProperties.generated.h"


class UAnimMontage;

UENUM(BlueprintType)
enum EWeaponType
{
	EWT_Rifle       UMETA(DisplayName = "Rifle"),
	EWT_Pistol      UMETA(DisplayName = "Pistol"),
	EWT_Grenade     UMETA(DisplayName = "Grenade"),
	EWT_Melle       UMETA(DisplayName = "Melle"),
};

USTRUCT(BlueprintType)
struct FBaseWeaponProperties : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon")
	TEnumAsByte<EWeaponType> WeaponType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseWeapon",
			  meta = (EditCondition = "WeaponType == EWeaponType::EWT_Grenade", EditConditionHides))
	TObjectPtr<UAnimMontage> Throw_Montage;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = 1, ClampMax = 100))
	float BaseDamage = 0.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon",
		meta = (DisplayName = "Attack range, m"))
	float AttackRange = 10.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon")
	float AttackRadius = 2.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon")
	FVector AimingCameraOffset;
};
