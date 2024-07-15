#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem/BaseWeapon.h"
#include "Components/TimelineComponent.h"
#include "FireWeapon.generated.h"

#pragma region TODO
/*
	- Add a camera shake effect for recoil
	- Add a projectiles
	- Add a aim offset rotation for character mesh to camera aim point
	- Add a weapon slide sliding
*/
#pragma endregion

UENUM(BlueprintType)
enum EFireMode {
	EFM_None,
	EFM_Single,
	EFM_Burst,
	EFM_FullAuto
};

UCLASS()
class INVENTORYANDWEAPONSYSTEM_API AFireWeapon : public ABaseWeapon
{
	GENERATED_BODY()

#pragma region Fire weapon properties
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon",
		meta = (ClampMin = 0))
	int MagazineSize;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon",
		meta = (ClampMin = 1))
	int PelletsPerShoot;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon",
		meta = (ClampMin = 0))
	float MaxSpread;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon")
	bool bAllowAutoFire = true;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon")
	TSubclassOf<class ABullet> BulletClass;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon")
	float FireRate;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon")
	float Strenght;
	/*This vector needs if weapon mesh is rotating, to determine recoil direction*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil")
	FVector KickBack_Direction;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil")
	float KickBackAmount;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil")
	double RecoilInterpSpeed;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil")
	double RecoilRecoverInterpSpeed;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil")
	class UCurveVector* RecoilRotation_Curve = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil")
	float RecoilYawMin;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil")
	float RecoilYawMax;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil",
		meta = (ClampMin = 0.f))
	float RecoilPitchMin;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil",
		meta = (ClampMin = 0.f))
	float RecoilPitchMax;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|FireWeapon|Recoil")
	TSubclassOf<UCameraShakeBase> RecoilCameraShake_Class;
	UPROPERTY(BlueprintReadWrite, Category = "Debug")
	bool bToogleCameraShake = true;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
	class UParticleSystemComponent* MuzzleFlash;

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
	/*Timeline for get recoil rotation vector curve values*/
	FTimeline RecoilRotaionTimeline;
	/*Check if weapon is ready to fire new projectile*/
	bool bReadyToShoot;
	class UAnimInstance* AnimBP = NULL;

private:

	/*Check if weapon able to firing*/
	bool CheckIfCanShoot() { return bReadyToShoot && CurrentAmmo > 0; };
	/*Reset a weapon from last shoot*/
	void ResetShoot();

	/*Move and rotate weapon mesh with arms, connected with weapon by IK*/
	void Recoil();
	/*Spreds a bullets in a random direction*/
	FVector Spread();

protected:

	UPROPERTY(BlueprintReadOnly)
	int CurrentAmmo;

	/*Recoil weapon mesh rotation function for timeline*/
	UFUNCTION()
	void RotateMesh();

	/*Shoot one projectile*/
	UFUNCTION()
	void Fire();

#pragma endregion

public:

	AFireWeapon();

	UFUNCTION()
	void BeginPlay() override;

	UFUNCTION()
	void Tick(float DeltaSeconds) override;


	int GetBulletsLeft() const {
		return CurrentAmmo;
	};

protected:

	void Attack() override;
	void StopAttack() override;

	UFUNCTION(BlueprintCallable)
	void Reload();
	
};


UCLASS()
class INVENTORYANDWEAPONSYSTEM_API ABullet : public AActor {

	GENERATED_BODY()

protected:

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* BulletMesh;

	UPROPERTY()
	class UProjectileMovementComponent* ProjectileMovement;

public:

	ABullet();

	void BeginPlay() override;

	UFUNCTION()
	void Throw(const FVector Direction);

	UFUNCTION()
	void HitEvent(UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

};
