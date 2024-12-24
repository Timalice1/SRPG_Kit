#include "WeaponSystem/FireWeapon.h"
#include "Curves/CurveVector.h"
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include <DrawDebugHelpers.h>
#include <Particles/ParticleSystemComponent.h>
#include "WeaponSystem/CombatInterface.h"
#include "DamageableInterface.h"
#include "Sound/SoundCue.h"
#include "../../Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "Perception/AISense_Hearing.h"
#include "PhysicsEngine\PhysicsSettings.h"

#pragma region FireWeapon

AFireWeapon::AFireWeapon()
{

	AimPoint = CreateDefaultSubobject<USceneComponent>("AimPoint");
	AimPoint->SetRelativeRotation(Mesh->GetRelativeRotation());
	AimPoint->SetupAttachment(RootPoint);

	BlockPoint = CreateDefaultSubobject<USceneComponent>("BlockingPoint");
	BlockPoint->SetupAttachment(RootPoint);

	LeftHand = CreateDefaultSubobject<USceneComponent>("LeftHand");
	LeftHand->SetupAttachment(Mesh);
	RightHand = CreateDefaultSubobject<USceneComponent>("RightHand");
	RightHand->SetupAttachment(Mesh);

	WallBlockPivot = CreateDefaultSubobject<USceneComponent>("WallBlockPivotPoint");
	WallBlockPivot->SetupAttachment(RootPoint);
	Mesh->SetupAttachment(WallBlockPivot);

	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>("MuzzleFlash");
	MuzzleFlash->SetupAttachment(Mesh);

	ShellEject_System = CreateDefaultSubobject<UNiagaraComponent>("ShellejectSystem");
	ShellEject_System->SetupAttachment(Mesh);

	bInfiniteBullets = false;
}

void AFireWeapon::BeginPlay()
{
	_weaponProperties = WeaponPropertiesRow.GetRow<FFireWeaponFroperties>(FString());
	if (!_weaponProperties)
	{
		UE_LOG(WeaponLog, Warning, TEXT("[%s]: row not found"), *GetName());
		Super::BeginPlay();
		return;
	}

	CurrentAmmo = _weaponProperties->MagazineSize;
	AnimBP = Mesh->GetAnimInstance();

	// Bind Recoil rotation timeline
	if (_weaponProperties->RecoilRotation_Curve != nullptr)
	{
		FOnTimelineVector timelineCallback;
		timelineCallback.BindUFunction(this, FName("RotateMesh"));
		RecoilRotaion_Timeline.AddInterpVector(_weaponProperties->RecoilRotation_Curve, timelineCallback);
	}

	// Bind recoil translation timeline
	if (_weaponProperties->RecoilTranslation_Curve != nullptr)
	{
		FOnTimelineVector timelineCallback;
		timelineCallback.BindUFunction(this, FName("TranslateMesh"));
		RecoilTranslation_Timeline.AddInterpVector(_weaponProperties->RecoilTranslation_Curve, timelineCallback);
	}

	if (MuzzleFlash != nullptr && MuzzleFlash->IsActive())
		MuzzleFlash->Deactivate();

	if (ShellEject_System != nullptr && ShellEject_System->IsActive())
		ShellEject_System->Deactivate();

	_fireRate = 60 / static_cast<float>(_weaponProperties->RPM);
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString::Printf(TEXT("FireRate: %f"), _fireRate));

	Super::BeginPlay();
}

void AFireWeapon::Tick(float DeltaSeconds)
{
	if (!_weaponProperties)
	{
		Super::Tick(DeltaSeconds);
		return;
	}

	// Update timelines
	RecoilRotaion_Timeline.TickTimeline(DeltaSeconds);
	RecoilTranslation_Timeline.TickTimeline(DeltaSeconds);

	CheckBlocking();

	Super::Tick(DeltaSeconds);
}

void AFireWeapon::CheckBlocking()
{
	FHitResult _hit;

	if (bCheckWallBlock)
	{
		FVector _start = GetActorLocation();
		FVector _end = BlockPoint->GetComponentLocation();
		TArray<AActor *> _ignore;
		_ignore.Add(Execute_GetOwningCharacter(this));

		UKismetSystemLibrary::SphereTraceSingle(
			this, _start, _end, _weaponProperties->TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Camera),
			false, _ignore,
			_bDrawDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			_hit, true);
	}

	if (_hit.bBlockingHit && !_hit.bStartPenetrating)
	{
		bBlocked = true;

		float _rotationAmount = UKismetMathLibrary::Vector_Distance(BlockPoint->GetComponentLocation(), GetActorLocation()) - _hit.Distance;
		_rotationAmount *= _weaponProperties->RotationMultiplier;
		_rotationAmount = FMath::Clamp(_rotationAmount, _weaponProperties->RotationMin, _weaponProperties->RotationMax);
		FVector _tmp = _weaponProperties->RotationVector * _rotationAmount;

		InterpPivotPointTransform(_weaponProperties->TranslationMax, FRotator(_tmp.X, _tmp.Y, _tmp.Z));
		return;
	}

	InterpPivotPointTransform(FVector(0, 0, 0), FRotator(0, 0, 0));
	bBlocked = false;
}

void AFireWeapon::InterpPivotPointTransform(FVector NewTranslation, FRotator NewRotation)
{
	float _deltaTime = GetWorld()->GetDeltaSeconds();

	WallBlockPivot->SetRelativeRotation(UKismetMathLibrary::RInterpTo(
		WallBlockPivot->GetRelativeRotation(),
		NewRotation,
		_deltaTime,
		_weaponProperties->InterpSpeed));
	WallBlockPivot->SetRelativeLocation(UKismetMathLibrary::VInterpTo(
		WallBlockPivot->GetRelativeLocation(),
		NewTranslation,
		_deltaTime,
		_weaponProperties->InterpSpeed));
}

void AFireWeapon::StartAttack_Implementation()
{
	if (!_weaponProperties)
		return;

	// Break this function
	// if weapon not reseted for new shoot
	if (!bReadyToShoot || isReloading || !bReseted || bBlocked)
		return;

	// Check if not out of ammo
	if (CurrentAmmo == 0)
	{
		this->PlaySound(_weaponProperties->Dry_Sound, GetActorLocation());
		bReadyToShoot = true;
		return;
	}

	Fire();
	return;
}

void AFireWeapon::StopAttack_Implementation()
{
	bReseted = true;
}

/*
 * Add some bullet spread for weapon
 * Calculates random values from a given ranges
 */
FVector AFireWeapon::GetBulletSpread() const
{
	float randX = UKismetMathLibrary::RandomFloatInRange(-_weaponProperties->MaxSpread, _weaponProperties->MaxSpread);
	float randY = UKismetMathLibrary::RandomFloatInRange(-_weaponProperties->MaxSpread, _weaponProperties->MaxSpread);
	float randZ = UKismetMathLibrary::RandomFloatInRange(-_weaponProperties->MaxSpread, _weaponProperties->MaxSpread);

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
	bReseted = false;

	if (CurrentAmmo == 0)
	{
		this->PlaySound(_weaponProperties->Dry_Sound, GetActorLocation());
		bReadyToShoot = true;
		return;
	}

	// Define projectile spawn location
	FVector muzzleLocation = Mesh->GetSocketLocation("Muzzle");
	FRotator muzzleRotation = Mesh->GetSocketRotation("Muzzle");
	FVector Direction = FVector();

	// Do loop for each pellet per shoot
	//(Spawn a N projectiles per shoot)
	if (Projectile != nullptr)
	{
		for (int i = 0; i < _weaponProperties->PelletsPerShoot; i++)
		{
			/*Define bullet throw direction*/
			FVector _start = muzzleLocation;
			FVector _end = (UKismetMathLibrary::GetRightVector(muzzleRotation) * _weaponProperties->AttackRange*100) +
						   _start;
			_end += GetBulletSpread();

			FHitResult _hit;
			UKismetSystemLibrary::SphereTraceSingle(this, _start, _end, _weaponProperties->AttackRadius,
													UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor *>(),
													EDrawDebugTrace::None, _hit, true);

			FVector _target = _hit.bBlockingHit ? _hit.Location : _hit.TraceEnd;
			float _len = _hit.Distance;
			float _t = _len/(_weaponProperties->BulletSpeed * 100);
			float _horizontalVelocity = (_len - (0.5f * (_t * _t) * (UPhysicsSettings::Get()->DefaultGravityZ / 2))) / _t;

			FVector  unitDirection=
				UKismetMathLibrary::GetDirectionUnitVector(muzzleLocation, _target);
			unitDirection *= _horizontalVelocity;

			/*Spawn and throw new bullet*/
			AProjectile *bullet = GetWorld()->SpawnActor<AProjectile>(Projectile, muzzleLocation, muzzleRotation);
			bullet->SetReboundProbability(_weaponProperties->ReboundProbability);
			bullet->SetDamageAmount(_weaponProperties->BaseDamage);
			bullet->SetImpulse(_weaponProperties->BulletHitImpulse);
			bullet->Throw(unitDirection, Execute_GetOwningCharacter(this));
		}
	}

	if (!bInfiniteBullets && CurrentAmmo > 0)
		CurrentAmmo = FMath::Max(0, CurrentAmmo - 1);

	/*Activate muzzle flash component*/
	if (MuzzleFlash != nullptr)
		MuzzleFlash->Activate(true);

	if (ShellEject_System != nullptr)
		ShellEject_System->Activate(true);

	this->PlaySound(_weaponProperties->Shoot_Sound, GetActorLocation());
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, _weaponProperties->NoiseRange, TEXT("Shooting"));

	AFireWeapon::Recoil();

	bReseted = _weaponProperties->bAllowAutoFire;

	/** Reset weapon after one shoot after delay */
	FTimerHandle _resetTimer;
	GetWorld()->GetTimerManager().SetTimer(_resetTimer, this, &ThisClass::ResetShoot, _fireRate);
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
	float randPitch = -(UKismetMathLibrary::RandomFloatInRange(_weaponProperties->RecoilPitchMin, _weaponProperties->RecoilPitchMax));
	float randYaw = UKismetMathLibrary::RandomFloatInRange(_weaponProperties->RecoilYawMin, _weaponProperties->RecoilYawMax);

	TObjectPtr<ACharacter> _owner = Execute_GetOwningCharacter(this);
	if (_owner != nullptr)
	{
		if (_owner != nullptr)
		{
			_owner->AddControllerPitchInput(randPitch * GetWorld()->GetDeltaSeconds());
			_owner->AddControllerYawInput(randYaw * GetWorld()->GetDeltaSeconds());
		}
	}
	else
		logger.Error(FText::FromString(FString::Printf(TEXT("%s: Character owner is nullptr"), *GetName())));

	/*Add camera shake effect for recoil*/
	if (_weaponProperties->RecoilCameraShake_Class != nullptr && _weaponProperties->bToogleCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(this, _weaponProperties->RecoilCameraShake_Class, GetActorLocation(), 350.f, 450.f);
	}
}

void AFireWeapon::RotateMesh()
{
	float timelineValue = RecoilRotaion_Timeline.GetPlaybackPosition();
	FVector curveValue = _weaponProperties->RecoilRotation_Curve->GetVectorValue(timelineValue) * -1 * _weaponProperties->RecoilRotation_Scale;
	curveValue *= GetWorld()->GetDeltaSeconds();

	FRotator recoilRotation = FRotator(
		curveValue.X,
		curveValue.Z,
		curveValue.Y);
	Mesh->SetRelativeRotation(recoilRotation);
}

void AFireWeapon::TranslateMesh()
{
	float timelineValue = RecoilTranslation_Timeline.GetPlaybackPosition();
	FVector curveValue = _weaponProperties->RecoilTranslation_Curve->GetVectorValue(timelineValue) * _weaponProperties->RecoilTranslation_Scale;
	curveValue *= GetWorld()->GetDeltaSeconds();

	Mesh->SetRelativeLocation(curveValue);
}

void AFireWeapon::ReloadStart_Implementation()
{
	if (!_weaponProperties)
		return;
	isReloading = true;
}

void AFireWeapon::ReloadEnd_Implementation(bool IsInterrupted)
{
	if (!IsInterrupted)
		CurrentAmmo = _weaponProperties->MagazineSize;
	isReloading = false;
	return;
}

#pragma endregion FireWeapon

#pragma region Projectile

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetLifeSpan(2.f);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMesh->SetGenerateOverlapEvents(true);
	ProjectileMesh->SetCollisionProfileName("Projectile");
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	Tracer = CreateDefaultSubobject<UParticleSystemComponent>("Tracer");
	Tracer->AttachToComponent(ProjectileMesh, FAttachmentTransformRules::KeepRelativeTransform);

	Rebound = 0.f;
	ProjectileDamageMultiplyer = 1.f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer != nullptr)
		Tracer->Activate();
}

void AProjectile::Throw(const FVector Direction, ACharacter *DamageCauser)
{
	ProjectileMovement->Velocity = Direction;
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::HitEvent);
	this->Attacker = DamageCauser;
}

void AProjectile::HitEvent(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{

	/*Apply damage if other actor is damageable*/

	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{

		/*Make this function return bool in case if we killed character or not instead of damaged*/
		/*Make Damage causer to be character controller instead or beside character class*/
		bool damaged = IDamageableInterface::Execute_TakeDamage(
			OtherActor,
			Damage * ProjectileDamageMultiplyer,
			Hit,
			EDamageType::BulletDamage,
			Attacker);
	}

	/*Apply impulse to hitted actor*/
	FVector _impulse = UKismetMathLibrary::GetRightVector(GetActorRotation()) * Impulse;
	if (OtherComp->IsAnySimulatingPhysics())
		OtherComp->AddImpulse(_impulse, Hit.BoneName, true);

	ProjectileMovement->bShouldBounce = Cast<ACharacter>(OtherActor) ? 0 : UKismetMathLibrary::RandomBoolWithWeight(Rebound);
	if (!ProjectileMovement->bShouldBounce)
		Destroy();

	Tracer->Deactivate();

	/*ADD SPAWN DECALS HERE AND SPAWN SOME HIT EMMITERS*/
}

#pragma endregion Projectile