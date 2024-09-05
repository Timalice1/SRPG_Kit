#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem/BaseWeapon.h"
#include "Components/TimelineComponent.h"
#include "FireWeaponInterface.h"
#include "CombatInterface.h"
#include "Logging/MessageLog.h"
#include "FireWeapon.generated.h"

class AProjectile;
class UParticleSystemComponent;
class UNiagaraComponent;

UENUM(BlueprintType)
enum EFireMode
{
	EFM_None,
	EFM_Single,
	EFM_Burst,
	EFM_FullAuto
};

UCLASS()
class SRPG_KIT_API AFireWeapon : public ABaseWeapon, public IFireWeaponInterface
{
	GENERATED_BODY()

	FMessageLog logger = FMessageLog(FName("PIE"));

#pragma region Fire weapon properties
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> AimPoint;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> LeftHand;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> RightHand;

	// Debuggind properties
	UPROPERTY(EditAnywhere, Category = "DEBUG")
	bool bInfiniteBullets;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
			  meta = (ClampMin = 0))
	int MagazineSize;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
			  meta = (ClampMin = 1))
	int PelletsPerShoot;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
			  meta = (ClampMin = 0))
	float MaxSpread;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon")
	bool bAllowAutoFire = true;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon",
			  meta = (ClampMin = 0, ClampMax = 1))
	float ReboundProbability;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon")
	TSubclassOf<AProjectile> Projectile;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon")
	float FireRate;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon")
	float Strenght;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil")
	class UCurveVector *RecoilRotation_Curve = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil")
	class UCurveVector *RecoilTranslation_Curve = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil",
			  meta = (ClampMin = "0"))
	float RecoilTranslation_Scale;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil",
			  meta = (ClampMin = "0"))
	float RecoilRotation_Scale;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon|Recoil")
	float RecoilRecoverInterpSpeed;
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

	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|ADS")
	bool bCanADS = true;
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|ADS",
			  meta = (EditCondition = "bCanADS", EditConditionHides))
	float ADS_TurnRate;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FireWeapon")
	FVector FP_CameraOffset = FVector(0, 0, 5.f);

	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon")
	TObjectPtr<UStaticMesh> MagazineMesh = nullptr;

	UFUNCTION()
	TArray<FName> GetBoneNames() const
	{
		return Mesh->GetAllSocketNames();
	}
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon", meta = (GetOptions = "GetBoneNames"))
	FName MagazineBoneName;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UParticleSystemComponent> MuzzleFlash;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> ShellEject_System;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FireWeapon|Animations")
	TObjectPtr<UAnimMontage> Reload_Montage;

#pragma endregion

#pragma region For internal use
private:
	/*Timer handler for burst/auto fire modes*/
	FTimerHandle FireTimer;
	/*Timer handler for shooting reset*/
	FTimerHandle ResetTimer;
	bool isReloading = false;
	TEnumAsByte<EFireMode> FireMode = EFM_FullAuto;
	/*Uses for storing default weapon mesh translation value for recoil*/
	FVector DefaultTranslation;
	/*Uses for storing default weapon mesh rotator value for recoil*/
	FRotator DefaultRotation;
	/*Timelines for weapon recoil*/
	FTimeline RecoilRotaion_Timeline;
	FTimeline RecoilTranslation_Timeline;
	/*Check if weapon is ready to fire new projectile*/
	bool bReadyToShoot = true;
	class UAnimInstance *AnimBP = NULL;

private:
	/*Reset a weapon from last shoot*/
	void ResetShoot();
	/*Move and rotate weapon mesh with arms, connected with weapon by IK*/
	void Recoil();

protected:
	int CurrentAmmo;
	UFUNCTION()
	void RotateMesh();
	UFUNCTION()
	void TranslateMesh();
	UFUNCTION()
	void Fire();

#pragma endregion

public:
	AFireWeapon();

	UFUNCTION()
	void BeginPlay() override;

	UFUNCTION()
	void Tick(float DeltaSeconds) override;

protected:
	void Attack_Implementation() override;

	void StopAttack_Implementation() override;

	FVector GetBulletSpread_Implementation() const override;

	FVector GetAimPointLocation_Implementation() const override
	{
		return AimPoint->GetComponentLocation();
	};

	bool CanADS_Implementation() const override
	{
		return bCanADS;
	};

	float GetADS_TurnRate_Implementation() const override
	{
		return ADS_TurnRate;
	};

	void GetHandsIK_Transform(const USkeletalMeshComponent *CharacterMesh,
							  FTransform &RightHandTransform,
							  FTransform &LeftHandTransform) const override;

	void ReloadStart_Implementation() override;

	void ReloadEnd_Implementation(bool IsInterrupted) override;

	bool CanBeReloaded_Implementation() const override
	{
		// Also add check for required ammo type in inventory
		return (!isReloading && CurrentAmmo < MagazineSize);
	};

	UStaticMesh *GetMagazineMesh_Implementation() const override
	{
		return MagazineMesh;
	};

	FName GetMagazineBoneName_Implementation() const override
	{
		FName name = "None";
		name = MagazineBoneName;
		return name;
	};
};

UCLASS()
class SRPG_KIT_API AProjectile : public AActor
{

	GENERATED_BODY()

	float Rebound = 0;
	float Damage = 10.f;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent *ProjectileMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent *ProjectileMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UParticleSystemComponent *Tracer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = 1.f))
	float ProjectileDamageMultiplyer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = "0", ClampMax = "1"))
	float ImpulseModifier = .1f;

public:
	AProjectile();
	void BeginPlay() override;
	void Throw(const FVector Direction);
	UFUNCTION()
	void HitEvent(UPrimitiveComponent *HitComponent,
				  AActor *OtherActor,
				  UPrimitiveComponent *OtherComp,
				  FVector NormalImpulse,
				  const FHitResult &Hit);

public:
	void SetReboundProbability(float Value)
	{
		this->Rebound = Value;
	}
	void SetDamageAmount(float Amount)
	{
		this->Damage = Amount;
	}
};
