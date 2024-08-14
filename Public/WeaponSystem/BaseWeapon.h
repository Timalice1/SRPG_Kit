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

	/*
		TODO:
		Move this function to a specific weapon interface
		(And MAke this interface)
	*/
	
	UFUNCTION(BlueprintCallable)
	void GetHandsIK_Transform(const USkeletalMeshComponent* CharacterMesh, 
		FName RightHandSocketName, 
		FName LeftHandSocketName,
		FTransform &RightHandTransform,
		FTransform &LeftHandTransform) const;

	UFUNCTION(BlueprintCallable)
	virtual void Attack() {};
	UFUNCTION(BlueprintCallable)
	virtual void StopAttack() {};
	
};
