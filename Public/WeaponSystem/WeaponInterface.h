#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SRPG_KIT_API IWeaponInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseWeapon")
	void Attack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseWeapon")
	void StopAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseWeapon")
	void Drop();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BaseWeapon")
	float GetAimingCameraOffset() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseWeapon")
	ACharacter *GetOwningCharacter() const;
};
