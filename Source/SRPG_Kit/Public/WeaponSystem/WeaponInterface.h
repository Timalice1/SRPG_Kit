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
	/**
	 * @param Message String with description of problem, if function return false.
	 * You can switch on string to determine next steps.
	 * Leave string empty if return state = true
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseWeapon")
	void StartAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseWeapon")
	void StopAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseWeapon")
	void Drop();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BaseWeapon")
	FVector GetAimingCameraOffset() const;

	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseWeapon")
	// ACharacter *GetOwningCharacter() const;

	virtual void GetWeaponProperties(struct FBaseWeaponProperties& properties) = 0;
};
