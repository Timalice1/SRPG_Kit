#pragma once

#include "BaseWeapon.h"
#include "Grenade.generated.h"

UCLASS(MinimalAPI)
class AGrenade : public ABaseWeapon
{
    GENERATED_BODY()

private: 
    bool _bReseted = true;

private:

    virtual FVector GetThrowDirection();

    virtual FVector CalculateVelocity(FVector target);

protected:

    UPROPERTY(EditDefaultsOnly, Category = Projectile)
    TSubclassOf<AGrenadeProjectile> _projectile;

    UPROPERTY(EditDefaultsOnly, Category = Projectile)
    float MaxVelocity = 1200.f;

    UPROPERTY(BlueprintReadWrite, Category = Grenade)
    bool bCanThrow = false;

protected:
    UFUNCTION(BlueprintCallable, Category = Grenade)
    virtual void Launch(FVector Target);

    UFUNCTION(BlueprintImplementableEvent, Category = Grenade)
    void ChargeThrow();

    void StartAttack_Implementation() override;

    void StopAttack_Implementation() override;

public:
    AGrenade() {};
};

UCLASS(MinimalAPI)
class AGrenadeProjectile : public AActor
{
    GENERATED_BODY()

public:
    AGrenadeProjectile();

protected:
    UPROPERTY(EditDefaultsOnly, Category = Components)
    TObjectPtr<class UStaticMeshComponent> Mesh = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = Components)
    TObjectPtr<class UExplosiveComponent> ExplosiveComponent = nullptr;

public:
    virtual UStaticMeshComponent* GetMesh() { return Mesh; };
    virtual UExplosiveComponent* GetExplosiveComponent() { return ExplosiveComponent; };
};