#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem/BaseWeapon.h"
#include "Components/TimelineComponent.h"
#include "FireWeaponInterface.h"
#include "CombatInterface.h"
#include "Logging/MessageLog.h"
#include "Kismet/GameplayStatics.h"
#include "Data/FireWeaponProperties.h"
#include "FireWeapon.generated.h"

class AProjectile;
class UParticleSystemComponent;
class UNiagaraComponent;
class USoundCue;

UENUM(BlueprintType)
enum EFireMode
{
	EFM_None,
	EFM_Single,
	EFM_Burst,
	EFM_FullAuto
};

/**Needs to be called whenewer one shoot is finished
 * to, f.ex. play animation on revolver or rifles
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResetShootEvent);

UCLASS()
class SRPG_KIT_API AFireWeapon : public ABaseWeapon, public IFireWeaponInterface
{
	GENERATED_BODY()

private:
	FMessageLog logger = FMessageLog(FName("PIE"));
	FFireWeaponFroperties *_weaponProperties;

	/*Ammo that left*/
	int32 CurrentAmmo;

	float _fireRate = 0;

	TEnumAsByte<EFireMode> FireMode = EFM_FullAuto;

	/*Timelines for weapon recoil*/
	FTimeline RecoilRotaion_Timeline;
	FTimeline RecoilTranslation_Timeline;

	/*Check if weapon is ready to fire new projectile*/
	bool bReadyToShoot = true;
	bool bReseted = true;
	bool isReloading = false;

	/** Anim instance reference from mesh */
	class UAnimInstance *AnimBP = NULL;

public:
	AFireWeapon();

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual void Fire();

	virtual void ResetShoot() { bReadyToShoot = true; };

	/*Move and rotate weapon mesh with arms, connected with weapon by IK*/
	UFUNCTION()
	virtual void Recoil();
	UFUNCTION()
	virtual void RotateMesh();
	UFUNCTION()
	virtual void TranslateMesh();

	/*Handle weapon mesh are blocked by obstacle*/
	virtual void CheckBlocking();

	virtual void PlaySound(USoundBase *Sound, FVector Location)
	{
		auto world = GetWorld();
		if (world != nullptr && Sound != nullptr)
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, Location);
	}

	virtual void InterpPivotPointTransform(FVector NewTranslation, FRotator NewRotation);

protected: // properties
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Components)
	TObjectPtr<USceneComponent> AimPoint;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<USceneComponent> BlockPoint;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<USceneComponent> LeftHand;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<USceneComponent> RightHand;

	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<USceneComponent> WallBlockPivot;

	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<UParticleSystemComponent> MuzzleFlash;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<UNiagaraComponent> ShellEject_System;

	UPROPERTY(EditDefaultsOnly, Category = FireWeapon)
	TSubclassOf<AProjectile> Projectile;

	// Debuggind properties
	UPROPERTY(EditAnywhere, Category = "FireWeapon|Debug")
	bool bInfiniteBullets;

	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	bool _bDrawDebug = false;

public:
	UPROPERTY(BlueprintReadWrite, Category = "FireWeapon|WallBlock")
	bool bCheckWallBlock = false;
	UPROPERTY(BlueprintReadOnly, Category = "FireWeapon|WallBlock")
	bool bBlocked = false;

protected: // Weapon interface implementation
	void StartAttack_Implementation() override;

	void StopAttack_Implementation() override;


	FVector GetAimPointLocation_Implementation() const override { return AimPoint->GetComponentLocation(); };

	FRotator GetAimPointRotation_Implementation() const override { return AimPoint->GetComponentRotation(); }

	bool CanADS_Implementation() const override { return _weaponProperties && _weaponProperties->bCanADS; };

	virtual bool IsBlocked_Implementation() const override { return bBlocked; }

	float GetADS_TurnRate_Implementation() const override { return _weaponProperties ? _weaponProperties->ADS_TurnRate : 1; };

	void GetHandsIK_Transform(const USkeletalMeshComponent *CharacterMesh,
							  FTransform &RightHandTransform,
							  FTransform &LeftHandTransform) const override;


	void ReloadStart_Implementation() override;

	void ReloadEnd_Implementation(bool IsInterrupted) override;

	bool CanBeReloaded_Implementation() const override
	{
		if (_weaponProperties)
			return (!isReloading && CurrentAmmo < _weaponProperties->MagazineSize);
		return false;
	};

	int32 GetCurrentAmmo_Implementation() const override { return CurrentAmmo; };

	float GetCurrentAmmoPercent_Implementation() override { return _weaponProperties ? static_cast<float>(CurrentAmmo) / static_cast<float>(_weaponProperties->MagazineSize) : 0; };

	UAnimMontage *GetReloadMontage_Implementation() override { return _weaponProperties->Reload_Montage; };

	FVector GetBulletSpread() const override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDamaged);

UCLASS(MinimalAPI)
class AProjectile : public AActor
{
	GENERATED_BODY()

private:
	float Rebound = 0;
	float Damage = 10.f;

	UPROPERTY()
	TObjectPtr<AActor> Attacker;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent *ProjectileMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent *ProjectileMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UParticleSystemComponent *Tracer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = 1.f))
	float ProjectileDamageMultiplyer;
	
	float Impulse = 1.f;

public:
	/*Calls whenewer bullet is damage a character*/
	FOnCharacterDamaged OnCharacterDamaged;

	AProjectile();

	virtual void BeginPlay() override;

	virtual void Throw(const FVector Direction, class ACharacter *DamageCauser);

	virtual void SetReboundProbability(float Value) { this->Rebound = Value; }

	virtual void SetDamageAmount(float Amount) { this->Damage = Amount; }

	virtual void SetImpulse(float Value) { this->Impulse = Value; };

	UFUNCTION()
	void HitEvent(UPrimitiveComponent *HitComponent,
				  AActor *OtherActor,
				  UPrimitiveComponent *OtherComp,
				  FVector NormalImpulse,
				  const FHitResult &Hit);
};
