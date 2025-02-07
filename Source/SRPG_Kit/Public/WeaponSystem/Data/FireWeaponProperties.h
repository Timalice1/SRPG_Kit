#pragma once

#include "BaseWeaponProperties.h"
#include "FireWeaponProperties.generated.h"

USTRUCT(BlueprintType)
struct FFireWeaponProperties : public FBaseWeaponProperties
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
			  meta = (ClampMin = -1))
	int32 MagazineSize = 30;
	/*Define bullet pellets per one shoot. 1 by default. Needs for shotguns f.ex.*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
			  meta = (ClampMin = 1))
	int PelletsPerShoot = 1;
	/*Max allowed bullet spread*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
			  meta = (ClampMin = 0))
	float MaxSpread = 1.f;
	/*Define if weapon can shoot burst or full auto*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon")
	bool bAllowAutoFire = true;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
			  meta = (ClampMin = 0, ClampMax = 1))
	float ReboundProbability = 0.1f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
		meta = (ClampMin = 1))
	int32 RPM = 1;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon", meta = (DisplayName = "Bullet Speed (m/s)"))
	float BulletSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = FireWeapon)
	float BulletHitImpulse = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil")
	class UCurveVector *RecoilRotation_Curve = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil")
	class UCurveVector *RecoilTranslation_Curve = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil",
			  meta = (ClampMin = "0"))
	float RecoilTranslation_Scale = 1.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil",
			  meta = (ClampMin = "0"))
	float RecoilRotation_Scale = 1.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil")
	float RecoilYawMin;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil")
	float RecoilYawMax;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil",
			  meta = (ClampMin = 0.f))
	float RecoilPitchMin;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil",
			  meta = (ClampMin = 0.f))
	float RecoilPitchMax;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil")
	TSubclassOf<UCameraShakeBase> RecoilCameraShake_Class;
	UPROPERTY(BlueprintReadWrite, Category = "Debug")
	bool bToogleCameraShake = true;

	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon")
	float NoiseRange = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|ADS")
	bool bCanADS = true;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|ADS",
			  meta = (EditCondition = "bCanADS", EditConditionHides))
	float ADS_TurnRate = .5f;

	/*Define camera offset for weapon in FP perspective*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon")
	FVector FP_CameraOffset = FVector(0, 0, 5.f);

    /*Weapon reload animation for character*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FireWeapon|Animations")
	TObjectPtr<UAnimMontage> Reload_Montage;

	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|Sounds")
	TObjectPtr<USoundCue> Shoot_Sound;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|Sounds")
	TObjectPtr<USoundBase> Dry_Sound;

	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	FVector RotationVector;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	float RotationMin;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	float RotationMax;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	float RotationMultiplier = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	FVector TranslationMax;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	float TraceRadius = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	float InterpSpeed;
};