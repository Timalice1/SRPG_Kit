#include "WeaponSystem/Grenade.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WeaponSystem/ExplosiveComponent.h"
#include "PhysicsEngine\PhysicsSettings.h"

FVector AGrenade::GetThrowDirection()
{
    if (!GetOwner())
        return FVector();

    FVector _start = GetActorLocation();
    FVector _end = (UKismetMathLibrary::GetForwardVector(Cast<ACharacter>(GetOwner())->GetBaseAimRotation()) * _weaponProperties->AttackRange * 100) + _start;

    FHitResult _hit;
    TArray<AActor *> _ignore;
    _ignore.Add(GetOwner());

    UKismetSystemLibrary::LineTraceSingle(
        this, _start, _end, UEngineTypes::ConvertToTraceType(ECC_Visibility),
        false, _ignore, EDrawDebugTrace::ForDuration, _hit, true);

    return _hit.bBlockingHit ? _hit.Location : _hit.TraceEnd;
}

FVector AGrenade::CalculateVelocity(FVector target)
{
    if (!GetOwner())
        return FVector();

    float _len = (target - GetActorLocation()).Size();
    float _t = _len / MaxVelocity;

    float _horizontalVelocity =
        (_len - (0.5f * (_t * _t) * (UPhysicsSettings::Get()->DefaultGravityZ / 2))) / _t;

    FVector _direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), target) * _horizontalVelocity;

    float _verticalVelocity =
        Cast<ACharacter>(GetOwner())->IsPlayerControlled() ? _direction.Z : (_t * (UPhysicsSettings::Get()->DefaultGravityZ / 2) * (-.5f));

    return FVector(
        _direction.X,
        _direction.Y,
        _verticalVelocity);
}

void AGrenade::StartAttack_Implementation()
{
    if (!_bReseted || !GetOwner())
        return;

    _bReseted = false;
    bCanThrow = false;

    ChargeThrow();
}

void AGrenade::StopAttack_Implementation()
{
    _bReseted = true;
}

void AGrenade::Launch(FVector Target)
{
    if (!bCanThrow)
        return;

    AGrenadeProjectile *_proj = GetWorld()->SpawnActor<AGrenadeProjectile>(_projectile, GetActorTransform());

    if (_proj)
    {
        _proj->SetOwner(GetOwner());
        _proj->GetMesh()->SetPhysicsLinearVelocity(
            CalculateVelocity(Target));
        _proj->GetExplosiveComponent()->Explode(_weaponProperties->BaseDamage, GetOwner());
    }
}

AGrenadeProjectile::AGrenadeProjectile()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
    RootComponent = Mesh;
    ExplosiveComponent = CreateDefaultSubobject<UExplosiveComponent>("ExplosiveComponent");
}