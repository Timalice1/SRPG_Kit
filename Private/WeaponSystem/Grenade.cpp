#include "WeaponSystem/Grenade.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WeaponSystem/ExplosiveComponent.h"
#include "PhysicsEngine\PhysicsSettings.h"

FVector AGrenade::GetThrowDirection()
{
    if (!_characterOwner)
        return FVector();

    FVector _start = GetActorLocation();
    FVector _end = (UKismetMathLibrary::GetForwardVector(_characterOwner->GetBaseAimRotation()) * _weaponProperties->AttackRange*100) + _start;

    FHitResult _hit;
    TArray<AActor *> _ignore;
    _ignore.Add(_characterOwner);

    UKismetSystemLibrary::LineTraceSingle(
        this, _start, _end, UEngineTypes::ConvertToTraceType(ECC_Visibility),
        false, _ignore, EDrawDebugTrace::ForDuration, _hit, true);

    return _hit.bBlockingHit ? _hit.Location : _hit.TraceEnd;
}

FVector AGrenade::CalculateVelocity(FVector target)
{
    float _len = (target - GetActorLocation()).Size();
    float _t = _len / MaxVelocity;

    float _horizontalVelocity =
        (_len - (0.5f * (_t * _t) * (UPhysicsSettings::Get()->DefaultGravityZ / 2))) / _t;

    FVector _direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), target) * _horizontalVelocity;

    float _verticalVelocity =
        _characterOwner->IsPlayerControlled() ? _direction.Z : (_t * (UPhysicsSettings::Get()->DefaultGravityZ / 2) * (-.5f));

    return FVector(
        _direction.X,
        _direction.Y,
        _verticalVelocity);
}

void AGrenade::StartAttack_Implementation()
{
    if (!_bReseted || !_characterOwner)
        return;

    _bReseted = false;
    bCanThrow = false;

    ChargeThrow();
}

void AGrenade::StopAttack_Implementation()
{
    _bReseted = true;
}

void AGrenade::Throw()
{
    if (!bCanThrow)
        return;

    AGrenadeProjectile *_proj = GetWorld()->SpawnActor<AGrenadeProjectile>(_projectile, GetActorTransform());

    _proj->GetMesh()->SetPhysicsLinearVelocity(
        CalculateVelocity(GetThrowDirection()));
    _proj->GetExplosiveComponent()->Explode(_weaponProperties->BaseDamage, Execute_GetOwningCharacter(this));
}

AGrenadeProjectile::AGrenadeProjectile()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
    RootComponent = Mesh;

    ExplosiveComponent = CreateDefaultSubobject<UExplosiveComponent>("ExplosiveComponent");
}