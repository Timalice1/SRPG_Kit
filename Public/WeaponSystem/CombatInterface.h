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

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FVector GetAttackDirection(const float Range);

	/*Return currently equipped weapon if it's valid*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	bool GetCurrentWeapon(ABaseWeapon* &CurrentWeapon);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	bool CanAttack() const;

};
