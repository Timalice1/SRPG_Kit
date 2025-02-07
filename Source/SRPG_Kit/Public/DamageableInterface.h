#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

UENUM(BlueprintType)
enum EDamageType
{
	BulletDamage,
	ExplosionDamage,
	Punch,
	Fire,
	Poison
};

UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class SRPG_KIT_API IDamageableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
	bool TakeDamage(float Amount, const FHitResult HitResult, EDamageType DamageType, class AActor *DamageCauser);
};
