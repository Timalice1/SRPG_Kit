#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "GameFramework/Character.h"
#include "WeaponInterface.h"
#include "Data/BaseWeaponProperties.h"
#include "BaseWeapon.generated.h"

#if WITH_EDITOR
DECLARE_LOG_CATEGORY_EXTERN(WeaponLog, Log, All);
#endif

class UAnimMontage;

UCLASS()
class SRPG_KIT_API ABaseWeapon : public ABaseItem, public IWeaponInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FDataTableRowHandle WeaponPropertiesRow;

	FBaseWeaponProperties *_weaponProperties;

public:
	ABaseWeapon();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void GetHandsIK_Transform(const USkeletalMeshComponent *CharacterMesh,
									  FTransform &RightHandTransform,
									  FTransform &LeftHandTransform) const {};

	void Drop_Implementation() override;

	UFUNCTION()
	FVector GetAimingCameraOffset_Implementation() const override
	{
		if (_weaponProperties)
			return _weaponProperties->AimingCameraOffset;
		return FVector(0, 0, 0);
	};

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	TEnumAsByte<EWeaponType> GetWeaponType() const
	{
		if (_weaponProperties)
			return _weaponProperties->WeaponType;
		return EWeaponType::EWT_Rifle;
	};

protected:
	UFUNCTION(BlueprintCallable, Category = BaseWeapon)
	virtual void GetWeaponProperties(struct FBaseWeaponProperties &properties) override
	{
		properties = *_weaponProperties;
	};
};
