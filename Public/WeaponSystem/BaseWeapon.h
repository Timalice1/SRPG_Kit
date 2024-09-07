#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "GameFramework/Character.h"
#include "WeaponInterface.h"
#include "BaseWeapon.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum EWeaponType
{
	Rifle,
	Pistol,
	Grenade,
	Melle,
};

UCLASS()
class SRPG_KIT_API ABaseWeapon : public ABaseItem, public IWeaponInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	TEnumAsByte<EWeaponType> WeaponType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon",
			  meta = (EditCondition = "WeaponType == EWeaponType::Grenade", EditConditionHides))
	TObjectPtr<UAnimMontage> Throw_Montage;
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	TObjectPtr<AActor> CharacterOwner;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon", meta = (ClampMin = 1, ClampMax = 100))
	float BaseDamage;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	float AttackRange;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	float AttackRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float AimingCameraOffset;

public:
	ABaseWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void GetHandsIK_Transform(const USkeletalMeshComponent *CharacterMesh,
									  FTransform &RightHandTransform,
									  FTransform &LeftHandTransform) const {};

	void Attack_Implementation() override {};

	void StopAttack_Implementation() override {};

	void Drop_Implementation() override;

	UFUNCTION()
	float GetAimingCameraOffset_Implementation() const override
	{
		return AimingCameraOffset;
	};

	UFUNCTION()
	AActor *GetOwningCharacter_Implementation() const override
	{
		return CharacterOwner;
	};

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	TEnumAsByte<EWeaponType> GetWeaponType() const
	{
		return WeaponType;
	};
};
