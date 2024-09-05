#include "WeaponSystem/FireWeapon.h"
#include "Curves/CurveVector.h"
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include <DrawDebugHelpers.h>
#include "Kismet/GameplayStatics.h"
#include <Particles/ParticleSystemComponent.h>
#include "WeaponSystem/CombatInterface.h"
#include "DamageableInterface.h"
#include "../../Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

#pragma region FireWeapon

AFireWeapon::AFireWeapon()
{

	AimPoint = CreateDefaultSubobject<USceneComponent>("AimPoint");
	AimPoint->SetRelativeRotation(Mesh->GetRelativeRotation());
	AimPoint->AttachToComponent(MeshRoot, FAttachmentTransformRules::KeepRelativeTransform);

	LeftHand = CreateDefaultSubobject<USceneComponent>("LeftHand");
	LeftHand->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	RightHand = CreateDefaultSubobject<USceneComponent>("RightHand");
	RightHand->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>("MuzzleFlash");
	MuzzleFlash->AttachToComponent(this->Mesh, FAttachmentTransformRules::KeepRelativeTransform);

	ShellEject_System = CreateDefaultSubobject<UNiagaraComponent>("ShellejectSystem");
	ShellEject_System->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);

	bInfiniteBullets = false;

	this->RecoilYawMin = .1f;
	this->RecoilYawMax = .1f;
	this->RecoilPitchMin = .1f;
	this->RecoilPitchMax = .1f;

	this->Strenght = 50000.f;

	this->bReadyToShoot = true;

	this->CurrentAmmo = 0;
	this->PelletsPerShoot = 1;
	this->MaxSpread = 1;

	this->FireRate = .15f;
	this->MagazineSize = 30;
	this->RecoilTranslation_Scale = 1;
	this->RecoilRotation_Scale = 1;
	this->RecoilRecoverInterpSpeed = 10.f;
	this->ReboundProbability = 0.f;

	ADS_TurnRate = .5f;
}

void AFireWeapon::BeginPlay()
{
	Super::BeginPlay();

	/*Store default mesh relative transform values*/
	DefaultRotation = Mesh->GetRelativeRotation();
	DefaultTranslation = Mesh->GetRelativeLocation();

	CurrentAmmo = MagazineSize;
	AnimBP = Mesh->GetAnimInstance();

	// Bind Recoil rotation timeline
	if (RecoilRotation_Curve != NULL)
	{
		FOnTimelineVector timelineCallback;
		timelineCallback.BindUFunction(this, FName("RotateMesh"));
		RecoilRotaion_Timeline.AddInterpVector(RecoilRotation_Curve, timelineCallback);
	}

	// Bind recoil translation timeline
	if (RecoilTranslation_Curve != NULL)
	{
		FOnTimelineVector timelineCallback;
		timelineCallback.BindUFunction(this, FName("TranslateMesh"));
		RecoilTranslation_Timeline.AddInterpVector(RecoilTranslation_Curve, timelineCallback);
	}

	if (MuzzleFlash != NULL && MuzzleFlash->IsActive())
		MuzzleFlash->Deactivate();

	if (ShellEject_System != NULL && ShellEject_System->IsActive())
		ShellEject_System->Deactivate();
}

void AFireWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Start timelines
	RecoilRotaion_Timeline.TickTimeline(DeltaSeconds);
	RecoilTranslation_Timeline.TickTimeline(DeltaSeconds);

	// Restore mesh translation to default
	FVector defaultTranslation = UKismetMathLibrary::VInterpTo(Mesh->GetRelativeLocation(),
															   DefaultTranslation,
															   GetWorld()->GetDeltaSeconds(),
															   RecoilRecoverInterpSpeed);
	Mesh->SetRelativeLocation(defaultTranslation);

	// Restore mesh rotation to default
	FRotator defaultRotation = UKismetMathLibrary::RInterpTo(Mesh->GetRelativeRotation(),
															 DefaultRotation,
															 GetWorld()->GetDeltaSeconds(),
															 RecoilRecoverInterpSpeed);
	Mesh->SetRelativeRotation(defaultRotation);
}

void AFireWeapon::Attack_Implementation()
{
	// Break this function
	// if weapon not reseted for new shoot
	if (!bReadyToShoot || isReloading)
		return;

	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &AFireWeapon::Fire, FireRate, bAllowAutoFire, 0);
}

void AFireWeapon::StopAttack_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

/*
 * Add some bullet spread for weapon
 * Calculates random values from a given ranges
 */
FVector AFireWeapon::GetBulletSpread_Implementation() const
{
	float randX = UKismetMathLibrary::RandomFloatInRange(-MaxSpread, MaxSpread);
	float randY = UKismetMathLibrary::RandomFloatInRange(-MaxSpread, MaxSpread);
	float randZ = UKismetMathLibrary::RandomFloatInRange(-MaxSpread, MaxSpread);

	return FVector(randX, randY, randZ);
}

void AFireWeapon::GetHandsIK_Transform(const USkeletalMeshComponent *CharacterMesh, FTransform &RightHandTransform, FTransform &LeftHandTransform) const
{
	FVector rightHand_location;
	FRotator rightHand_rotation;
	FVector leftHand_location;
	FRotator leftHand_rotation;

	FTransform rhs = RightHand->GetComponentTransform();
	FTransform lhs = LeftHand->GetComponentTransform();

	CharacterMesh->TransformToBoneSpace("ik_hand_gun",
										rhs.GetLocation(),
										rhs.Rotator(),
										rightHand_location,
										rightHand_rotation);

	CharacterMesh->TransformToBoneSpace("hand_r",
										lhs.GetLocation(),
										lhs.Rotator(),
										leftHand_location,
										leftHand_rotation);

	RightHandTransform = FTransform(rightHand_rotation, rightHand_location);
	LeftHandTransform = FTransform(leftHand_rotation, leftHand_location);
}

void AFireWeapon::Fire()
{

	bReadyToShoot = false;

	// Check if not out of ammo
	if (CurrentAmmo == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimer);
		return;
	}

	// Define projectile spawn location
	FVector muzzleLocation = Mesh->GetSocketLocation("Muzzle");
	FRotator muzzleRotation = Mesh->GetSocketRotation("Muzzle");
	FVector Direction = FVector();

	// Do loop for each pellet per shoot
	//(Spawn a N projectiles per shoot)
	if (Projectile != NULL)
	{
		for (int i = 0; i < PelletsPerShoot; i++)
		{

			/*Define bullet throw direction*/
			if (CharacterOwner != nullptr)
			{
				if (CharacterOwner->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
					Direction = ICombatInterface::Execute_GetAttackDirection(CharacterOwner, AttackRange, AttackRadius);
			}
			else
				logger.Error(FText::FromString(FString::Printf(TEXT("%s: Character owner is nullptr"), *GetName())));

			FVector unitDirection =
				UKismetMathLibrary::GetDirectionUnitVector(muzzleLocation, Direction);

			/*Spawn and throw new bullet*/
			AProjectile *bullet = GetWorld()->SpawnActor<AProjectile>(Projectile, muzzleLocation, muzzleRotation);
			bullet->SetReboundProbability(ReboundProbability);
			bullet->SetDamageAmount(BaseDamage);
			bullet->Throw(unitDirection * Strenght);
		}
	}

	if (!bInfiniteBullets && CurrentAmmo > 0)
		CurrentAmmo--;

	/*Activate muzzle flash component*/
	if (MuzzleFlash != NULL)
		MuzzleFlash->Activate(true);

	////Move weapon slider bone from mesh anim instance
	// TObjectPtr<UAnimInstance> animInst = Mesh->GetAnimInstance();
	// if (animInst != NULL && animInst->GetClass()->ImplementsInterface(UFireWeaponInterface::StaticClass()))
	//	IFireWeaponInterface::Execute_Slide(animInst, CurrentAmmo > 0);

	if (ShellEject_System != NULL)
		ShellEject_System->Activate(true);

	AFireWeapon::Recoil();
	/*Reset weapon for a new shoot after some delay*/
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AFireWeapon::ResetShoot, FireRate, false);
}

/*
 * Reset weapon firing ability to do a new shoot
 * Do it by defining weapon fire rate value and timers.
 */
void AFireWeapon::ResetShoot()
{
	bReadyToShoot = true;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Reset");
	GetWorld()->GetTimerManager().ClearTimer(ResetTimer);
}

/*
 * Update weapon mesh transform.
 * Move mesh back in defined direction with defined ammount
 * Use a Vector Curve and TimelineComponent to rotate the mesh and attached assets
 */
void AFireWeapon::Recoil()
{
	/*Start the recoil rotation timeline (rotates a weapon mesh around pivot point)*/
	RecoilRotaion_Timeline.PlayFromStart();
	RecoilTranslation_Timeline.PlayFromStart();

	// Add some recoil rotation to character controller by pitch and yae axis
	float randPitch = -(UKismetMathLibrary::RandomFloatInRange(RecoilPitchMin, RecoilPitchMax));
	float randYaw = UKismetMathLibrary::RandomFloatInRange(RecoilYawMin, RecoilYawMax);

	if (CharacterOwner != nullptr)
	{
		CharacterOwner->AddControllerPitchInput(randPitch * GetWorld()->GetDeltaSeconds());
		CharacterOwner->AddControllerYawInput(randYaw * GetWorld()->GetDeltaSeconds());
	}
	else
		logger.Error(FText::FromString(FString::Printf(TEXT("%s: Character owner is nullptr"), *GetName())));

	/*Add camera shake effect for recoil*/
	if (RecoilCameraShake_Class != NULL && bToogleCameraShake)
	{
		FVector cameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), RecoilCameraShake_Class, cameraLocation, 10.f, 10.f);
	}
}

void AFireWeapon::RotateMesh()
{
	float timelineValue = RecoilRotaion_Timeline.GetPlaybackPosition();
	FRotator currentRotation = Mesh->GetRelativeRotation();
	FVector curveValue = RecoilRotation_Curve->GetVectorValue(timelineValue) * -1 * RecoilRotation_Scale;
	curveValue *= GetWorld()->GetDeltaSeconds();

	FRotator recoilRotation = FRotator(
		currentRotation.Pitch + curveValue.X,
		currentRotation.Yaw + curveValue.Z,
		currentRotation.Roll + curveValue.Y);

	Mesh->SetRelativeRotation(recoilRotation);
}

void AFireWeapon::TranslateMesh()
{
	float timelineValue = RecoilTranslation_Timeline.GetPlaybackPosition();
	FVector curveValue = RecoilTranslation_Curve->GetVectorValue(timelineValue) * RecoilTranslation_Scale;
	FVector currentLocation = Mesh->GetRelativeLocation();
	curveValue *= GetWorld()->GetDeltaSeconds();

	FVector recoilLocation = FVector(
		currentLocation.X + curveValue.X,
		currentLocation.Y + curveValue.Y,
		currentLocation.Z + curveValue.Z);

	Mesh->SetRelativeLocation(recoilLocation);
}

void AFireWeapon::ReloadStart_Implementation()
{
	isReloading = true;
}

void AFireWeapon::ReloadEnd_Implementation(bool IsInterrupted)
{
	if (!IsInterrupted)
		CurrentAmmo = MagazineSize;
	isReloading = false;
}

#pragma endregion

#pragma region Projectile

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetLifeSpan(2.f);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMesh->SetGenerateOverlapEvents(true);
	ProjectileMesh->SetCollisionProfileName("Projectile");

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	Tracer = CreateDefaultSubobject<UParticleSystemComponent>("Tracer");
	Tracer->AttachToComponent(ProjectileMesh, FAttachmentTransformRules::KeepRelativeTransform);

	Rebound = 0.f;
	ProjectileDamageMultiplyer = 1.f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer != NULL)
		Tracer->Activate();
}

void AProjectile::Throw(const FVector Direction)
{
	ProjectileMovement->Velocity = Direction;
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::HitEvent);
}

void AProjectile::HitEvent(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{

	DrawDebugSphere(GetWorld(), Hit.Location, 10, 10, FColor::Green, false, 2.5f, 0, 1.f);

	/*Apply damage if other actor is damageable*/
	if (OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		bool damaged = IDamageableInterface::Execute_TakeDamage(OtherActor, Damage * ProjectileDamageMultiplyer, Hit, EDamageType::BulletDamage);
	}

	/*Apply impulse to hitted actor*/
	if (OtherComp->IsAnySimulatingPhysics())
		OtherComp->AddImpulse(ProjectileMovement->Velocity * ImpulseModifier, Hit.BoneName, true);

	ProjectileMovement->bShouldBounce = Cast<ACharacter>(OtherActor) ? 0 : UKismetMathLibrary::RandomBoolWithWeight(Rebound);
	if (!ProjectileMovement->bShouldBounce)
		Destroy();

	Tracer->Deactivate();

	/*ADD SPAWN DECALS HERE AND SPAWN SOME HIT EMMITERS*/
}

#pragma endregion