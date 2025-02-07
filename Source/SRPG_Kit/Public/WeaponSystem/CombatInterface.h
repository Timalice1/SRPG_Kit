#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SRPG_KIT_API ICombatInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface")
	void Attack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface")
	void StopAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface")
	FVector GetAttackDirection(const float Range = 10000.f, const float Radius = .1f);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface")
	bool CanAttack() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface|Weapon")
	void Aim();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface|Weapon")
	void StopAiming();

	/*Return currently equipped weapon if it's valid*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface")
	bool GetCurrentWeapon(ABaseWeapon *&CurrentWeapon) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface")
	void DropWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInterface|FireWeapon")
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface")
	bool EquipWeapon(int32 FromSlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatInterface")
	void HideWeapon();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Loadout")
	int32 GetAwailableSlotsNum();	
};
