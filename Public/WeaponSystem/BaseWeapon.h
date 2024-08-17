#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "GameFramework/Character.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum EWeaponType {
	FireWeapon,
	MelleWeapon
};

UCLASS()
class SRPG_KIT_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	ACharacter* CharacterOwner;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|Weapon", meta = (ClampMin = 1, ClampMax = 100))
	float BaseDamage;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|Weapon")
	float Range;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|Weapon")
	TEnumAsByte<EWeaponType> WeaponType;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|Weapon")
	bool bDoLeftHandIK = true;

public:

	ABaseWeapon();


	/*Maybe create a weapon interface and move all this stuff there.
	Make there generic implementation and extend it in blueprints*/
	
	UFUNCTION(BlueprintCallable)
	void GetHandsIK_Transform(const USkeletalMeshComponent* CharacterMesh, 
		FName RightHandSocketName, 
		FName LeftHandSocketName,
		FTransform &RightHandTransform,
		FTransform &LeftHandTransform) const;

	UFUNCTION(BlueprintCallable,Category = "Weapon")
	virtual void Attack() {};
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopAttack() {};
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	TEnumAsByte<EWeaponType> GetWeaponType() const {
		return WeaponType;
	};
};
