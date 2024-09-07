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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void Attack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void StopAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	FVector GetAttackDirection(const float Range = 10000.f, const float Radius = .1f);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	bool CanAttack() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void Aim();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void StopAiming();

	/*Return currently equipped weapon if it's valid*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
	bool GetCurrentWeapon(ABaseWeapon *&CurrentWeapon) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
	void DropWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FireWeapon")
	void ReloadWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage|Callback")
	void OnKillCharacter(class ACharacter *DamagedCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage|Callback")
	void OnDamageCharacter(class ACharacter *DamagedCharacter);
};
