#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};


/*
	The only thing this interface do, is to allowing actors to taking damage,
	providing communication between damage causer and damage target
*/
class SRPG_KIT_API IDamageableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool TakeDamage(float Amount, const FHitResult HitResult);

};
