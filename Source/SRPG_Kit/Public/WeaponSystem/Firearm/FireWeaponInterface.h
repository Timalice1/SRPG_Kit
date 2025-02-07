#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FireWeaponInterface.generated.h"

UINTERFACE(MinimalAPI)
class UFireWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class SRPG_KIT_API IFireWeaponInterface
{
	GENERATED_BODY()

public:
	virtual FVector GetBulletSpread() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|ADS")
	bool CanADS() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = FireWeapon)
	bool IsBlocked() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|ADS")
	float GetADS_TurnRate() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|ADS")
	FVector GetAimPointLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|ADS")
	FRotator GetAimPointRotation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|Reloading")
	bool CanBeReloaded() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|Reloading")
	void ReloadStart();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|Reloading")
	void ReloadEnd(bool IsInterrupted);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeaponInterface|Reloading")
	UAnimMontage* GetReloadMontage();

	// Property getters
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|WeaponData")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|WeaponData")
	float GetCurrentAmmoPercent();


};
