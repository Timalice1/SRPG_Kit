#include "WeaponSystem/BaseWeapon.h"
#include "Components/SphereComponent.h"

DEFINE_LOG_CATEGORY(WeaponLog);

ABaseWeapon::ABaseWeapon()
{
}

void ABaseWeapon::BeginPlay()
{
	if(WeaponPropertiesRow.IsNull())
	{
		UE_LOG(WeaponLog, Error, TEXT("[%s]: table row is null"), *GetName());
		return Super::BeginPlay();
	}
	_weaponProperties = WeaponPropertiesRow.GetRow<FBaseWeaponProperties>(FString());
	Super::BeginPlay();
}

void ABaseWeapon::Drop_Implementation()
{

	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh->SetSimulatePhysics(true);

	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Execute_StopAttack(this);

	SetLifeSpan(120);
}
