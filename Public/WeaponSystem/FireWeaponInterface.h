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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon")
	FVector GetBulletSpread() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|ADS")
	bool CanADS() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|ADS")
	float GetADS_TurnRate() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|ADS")
	FVector GetAimPointLocation() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|Reloading")
	bool CanBeReloaded() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|Reloading")
	void ReloadStart();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|Reloading")
	void ReloadEnd(bool IsInterrupted);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|Reloading")
	UStaticMesh *GetMagazineMesh() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeaponReloading")
	FName GetMagazineBoneName() const;

	// Property getters
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FireWeapon|WeaponData")
	int32 GetCurrentAmmo() const;
};
