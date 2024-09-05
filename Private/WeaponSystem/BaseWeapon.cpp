#include "WeaponSystem/BaseWeapon.h"
#include "Components/SphereComponent.h"

ABaseWeapon::ABaseWeapon()
{
	AttackRange = 10000.f;
	AttackRadius = .1f;
	BaseDamage = 1.f;

	AimingCameraOffset = -70.f;
}

void ABaseWeapon::Drop_Implementation()
{
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh->SetSimulatePhysics(true);

	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Mesh->AddImpulse(CharacterOwner->GetActorForwardVector() * 1000);

	Execute_StopAttack(this);

	SetLifeSpan(300);
}
