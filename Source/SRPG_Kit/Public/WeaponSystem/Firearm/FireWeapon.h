#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem/BaseWeapon.h"
#include "FireWeaponInterface.h"
#include "Components/TimelineComponent.h"
#include "FireWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResetShootEvent);

UCLASS()
class SRPG_KIT_API AFireWeapon : public ABaseWeapon, public IFireWeaponInterface
{
	GENERATED_BODY()

public:
	AFireWeapon();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, Category = "FireWeapon|WallBlock")
	bool bCheckWallBlock = false;
	UPROPERTY(BlueprintReadOnly, Category = "FireWeapon|WallBlock")
	bool bBlocked = false;

protected: // Components
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Components)
	TObjectPtr<class USceneComponent> AimPoint;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<class USceneComponent> BlockPoint;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<class USceneComponent> LeftHand;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<class USceneComponent> RightHand;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<class USceneComponent> WallBlockPivot;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<class USceneComponent> Muzzle;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<class UParticleSystemComponent> MuzzleFlash;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TObjectPtr<class UNiagaraComponent> ShellEject_System;

	UPROPERTY(EditDefaultsOnly, Category = FireWeapon)
	TSubclassOf<class AProjectile> Projectile;

private: // References
	class FMessageLog logger = FMessageLog(FName("PIE"));
	struct FFireWeaponProperties *_weaponProperties;
	
	UPROPERTY()
	class UFirearmAnimInstance *_animInstanceRef = nullptr;

private: //Default class properties
	FTimeline RecoilRotation_Timeline;
	FTimeline RecoilTranslation_Timeline;

	// Shooting properties
	bool bReadyToShoot = true;
	bool bReseted = true;
	bool isReloading = false;

	int32 CurrentAmmo;
	float _fireRate = 0;

private:
	virtual void BindRecoilTimelines();
	virtual void InitComponents();
	virtual void PlaySound(class USoundBase *Sound, FVector Location);

protected: // Fire weapon config
	// Debuggind properties
	UPROPERTY(EditDefaultsOnly, Category = "FireWeapon|WallBlock")
	bool _bDrawDebug = false;

private: // Shooting
	virtual void Fire();
	virtual bool SpawnProjectile();
	virtual void PlayShootFX();

private: // Recoil
	virtual void Recoil();
	UFUNCTION()
	virtual void RotateMesh();
	UFUNCTION()
	virtual void TranslateMesh();

private: // Wall blocking
	virtual void CheckBlocking();
	virtual void InterpPivotPointTransform(FVector NewTranslation, FRotator NewRotation);

protected: // Weapon interface implementation
	// Shooting
	void StartAttack_Implementation() override;
	void StopAttack_Implementation() override;

	// ADS
	FVector GetAimPointLocation_Implementation() const override { return AimPoint->GetComponentLocation(); }
	FRotator GetAimPointRotation_Implementation() const override { return AimPoint->GetComponentRotation(); }
	bool CanADS_Implementation() const override;
	float GetADS_TurnRate_Implementation() const override;

	// Wall blocking
	virtual bool IsBlocked_Implementation() const override { return bBlocked; }

	void GetHandsIK_Transform(const USkeletalMeshComponent *CharacterMesh,
							  FTransform &RightHandTransform,
							  FTransform &LeftHandTransform) const override;

	// Reloading
	void ReloadStart_Implementation() override;
	void ReloadEnd_Implementation(bool IsInterrupted) override;
	bool CanBeReloaded_Implementation() const override;

protected:
	UAnimMontage *GetReloadMontage_Implementation() override;
	int32 GetCurrentAmmo_Implementation() const override { return CurrentAmmo; };
	float GetCurrentAmmoPercent_Implementation() override;
	virtual FVector GetBulletSpread() const override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDamaged);
USTRUCT()
struct FImpact
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> impactSound;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> impactEmmiter;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> impactDecal;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "impactDecal!=nullptr", EditConditionHides))
	FVector decalScale = FVector(1, 1, 1);
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "impactDecal!=nullptr", EditConditionHides))
	float decalLifeSpan = 2.f;
};

UCLASS(MinimalAPI)
class AProjectile : public AActor
{
	GENERATED_BODY()

private:
	float Rebound = 0;
	float Damage = 10.f;
	float Impulse = 1.f;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent *ProjectileMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent *ProjectileMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UParticleSystemComponent *Tracer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = 1.f))
	float ProjectileDamageMultiplyer;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Impact")
	TMap<TEnumAsByte<EPhysicalSurface>, FImpact> _impactData;

public:
	/*Calls whenewer bullet is damage a character*/
	FOnCharacterDamaged OnCharacterDamaged;

	AProjectile();

	virtual void BeginPlay() override;
	virtual void Init(float ReboundProbability, float DamageAmount, float HitImpulse);
	virtual class UProjectileMovementComponent *GetProjectileMovement();

	UFUNCTION()
	void HitEvent(UPrimitiveComponent *HitComponent,
				  AActor *OtherActor,
				  UPrimitiveComponent *OtherComp,
				  FVector NormalImpulse,
				  const FHitResult &Hit);
};
