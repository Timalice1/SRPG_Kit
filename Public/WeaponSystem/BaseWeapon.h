#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "GameFramework/Character.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum EWeaponType {
	EWT_Pistol		UMETA(DisplayName = "Pistol"),
	EWT_Rifle		UMETA(DisplayName = "Rifle"),
};

UCLASS()
class INVENTORYANDWEAPONSYSTEM_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	ACharacter* CharacterOwner;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|Weapon")
	float Damage;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|Weapon")
	float Range;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|Weapon")
	TEnumAsByte<EWeaponType> WeaponType;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ItemProperties|Weapon")
	bool bDoLeftHandIK = true;

public:

	ABaseWeapon();

	/*
		TODO:
		Move this function to a specific weapon interface
		(And MAke this interface)
	*/
	
	/*
	* Returns a left hand socket transform value for left hand IK in bone space
	* !!Bones on weapon skeleton mus name as "LeftHandSocket" and "RightHandSocket"!!
	*/
	UFUNCTION(BlueprintCallable)
	void GetHandsIKTransform(const USkeletalMeshComponent* CharacterMesh,
		FName RightHandRelativeBone,
		FName LeftHandRelativeBone,
		FTransform& LeftHand,
		FTransform& RightHand) const;


	UFUNCTION(BlueprintCallable)
	virtual void Attack() {};
	UFUNCTION(BlueprintCallable)
	virtual void StopAttack() {};
	
};
