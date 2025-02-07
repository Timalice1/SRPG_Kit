#include "WeaponSystem/ExplosiveComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include <Kismet/GameplayStatics.h>
#include <DamageableInterface.h>
#include "GameFramework/Actor.h"
#include "Perception/AISense_Hearing.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"

UExplosiveComponent::UExplosiveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ExplosionDelay = .01f;
	ExplosionRadius = 750.f;
	ExplosionImpulse = 3000.f;
	ExplosionDamage = 75.f;

	ExplosionEmmiterScale = 1.f;
}

void UExplosiveComponent::Explode(float Damage, class AActor *DamageCauser)
{
	world = GetWorld();

	ExplosionDamage = Damage == 0 ? ExplosionDamage : Damage;
	damageInstigator = GetOwner();
	if (DamageCauser != nullptr)
		damageInstigator = DamageCauser;

	FTimerHandle explodeDelay;
	GetWorld()->GetTimerManager().SetTimer(explodeDelay, this, &UExplosiveComponent::Explode_Implementation, ExplosionDelay, false);
}

void UExplosiveComponent::Explode_Implementation()
{
	epicenter = GetOwner()->GetActorLocation();

	EDrawDebugTrace::Type debugTrace = bDrawDebugSphere ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	TArray<FHitResult> hits;

	// Check for each object in explosion radius
	UKismetSystemLibrary::SphereTraceMultiForObjects(world, epicenter, epicenter,
													 ExplosionRadius, ObjectTypes, false, IgnoreActors, debugTrace, hits, true);

	// Store hitted actor and their components in arrays
	auto actors = TArray<AActor *>();
	auto components = TArray<UPrimitiveComponent *>();
	auto bones = TArray<FName>();

	for (const FHitResult &hit : hits)
	{
		actors.AddUnique(hit.GetActor());
		components.AddUnique(hit.GetComponent());
		if (hit.BoneName != FName("root"))
		{
			bones.AddUnique(hit.BoneName);
		}
	}

	FHitResult nullHit;

	int i = 0;

	if (!actors.IsEmpty())
	{
		for (const auto &actor : actors)
		{
			// If actor implements damageable interface, apply explosion damage to this actor
			if (actor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) && actor != GetOwner())
			{
				IDamageableInterface::Execute_TakeDamage(
					actor,
					ExplosionDamage,
					nullHit,
					EDamageType::ExplosionDamage,
					damageInstigator);
			}

			// Othervise apply impusle to all other objects
			FVector explosionImpulse = UKismetMathLibrary::GetDirectionUnitVector(epicenter, actor->GetActorLocation());
			explosionImpulse.Z += .1f;
			explosionImpulse *= ExplosionImpulse;

			// Apply impulse to each bone we hit (if it is character)
			if (components[i]->IsAnySimulatingPhysics())
			{
				for (const FName &bone : bones)
					components[i]->AddImpulse(explosionImpulse, bone);
			}

			i++;
		}
	}

	if (world != nullptr)
	{
		if (ExplosionEmmiter != nullptr)
		{
			FTransform actorTransform = GetOwner()->GetActorTransform();
			FTransform emmiterTransform = FTransform(actorTransform.GetRotation(), actorTransform.GetLocation(), FVector(1, 1, 1) * ExplosionEmmiterScale);
			UGameplayStatics::SpawnEmitterAtLocation(world, ExplosionEmmiter, emmiterTransform);
		}

		UAISense_Hearing::ReportNoiseEvent(GetWorld(), epicenter, 1, GetOwner(), ExplosionNoiseRange, TEXT("Explosion"));
		if(ExplosionSound)
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, epicenter);

		if (ExplosionCameraShake_Class != nullptr)
			UGameplayStatics::PlayWorldCameraShake(world, ExplosionCameraShake_Class, epicenter, ExplosionRadius * 1.5f, ExplosionRadius);
	}

	GetOwner()->SetActorHiddenInGame(true);

	FTimerHandle _delay;
	GetWorld()->GetTimerManager().SetTimer(_delay, this, &ThisClass::DestroyOwner, DestroyTime, false);
}

void UExplosiveComponent::DestroyOwner()
{
	GetOwner()->Destroy();
}
