#include "WeaponSystem/Firearm/FireWeapon.h"
#include "WeaponSystem/Data/FireWeaponProperties.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Curves/CurveVector.h"
#include <Components/StaticMeshComponent.h>
#include "WeaponSystem/CombatInterface.h"
#include "DamageableInterface.h"
#include "NiagaraComponent.h"
#include <Particles/ParticleSystemComponent.h>
#include "Sound/SoundCue.h"
#include "Perception/AISense_Hearing.h"
#include "PhysicsEngine\PhysicsSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/MessageLog.h"
#include <Kismet/KismetMathLibrary.h>

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

	Muzzle = CreateDefaultSubobject<USceneComponent>("MuzzleComponent");
	Muzzle->SetupAttachment(Mesh);

	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>("MuzzleFlash");
	MuzzleFlash->SetupAttachment(Muzzle);

	ShellEject_System = CreateDefaultSubobject<UNiagaraComponent>("ShellejectSystem");
	ShellEject_System->SetupAttachment(Mesh);

	USceneComponent* _tmp = CreateDefaultSubobject<USceneComponent>("TMP");
}

void AFireWeapon::BeginPlay()
{
	_weaponProperties = WeaponPropertiesRow.GetRow<FFireWeaponProperties>(FString());
	if (!_weaponProperties)
	{
		UE_LOG(WeaponLog, Warning, TEXT("[%s]: row not found"), *GetName());
		return Super::BeginPlay();
	}

	InitComponents();
	BindRecoilTimelines();

	CurrentAmmo = _weaponProperties->MagazineSize;
	_fireRate = 60 / static_cast<float>(_weaponProperties->RPM);
	Super::BeginPlay();
}

void AFireWeapon::InitComponents()
{
	if (MuzzleFlash != nullptr && MuzzleFlash->IsActive())
		MuzzleFlash->Deactivate();

	if (ShellEject_System != nullptr && ShellEject_System->IsActive())
		ShellEject_System->Deactivate();
}

void AFireWeapon::BindRecoilTimelines()
{
	// Bind Recoil rotation timeline
	if (IsValid(_weaponProperties->RecoilRotation_Curve))
	{
		FOnTimelineVector timelineCallback;
		timelineCallback.BindUFunction(this, FName("RotateMesh"));
		RecoilRotation_Timeline.AddInterpVector(_weaponProperties->RecoilRotation_Curve, timelineCallback);
	}

	// Bind recoil translation timeline
	if (IsValid(_weaponProperties->RecoilTranslation_Curve))
	{
		FOnTimelineVector timelineCallback;
		timelineCallback.BindUFunction(this, FName("TranslateMesh"));
		RecoilTranslation_Timeline.AddInterpVector(_weaponProperties->RecoilTranslation_Curve, timelineCallback);
	}
}

void AFireWeapon::Tick(float DeltaSeconds)
{
	if (!_weaponProperties)
		return Super::Tick(DeltaSeconds);

	RecoilRotation_Timeline.TickTimeline(DeltaSeconds);
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
		_ignore.Add(GetOwner());

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
	if (!_weaponProperties || !bReadyToShoot || isReloading || !bReseted || bBlocked)
		return;
	Fire();
}

void AFireWeapon::StopAttack_Implementation()
{
	bReseted = true;
}

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

	FTimerHandle _resetTimer;
	GetWorldTimerManager().SetTimer(
		_resetTimer, [this]()
		{ bReadyToShoot = true; },
		_fireRate,
		false);

	if (CurrentAmmo == 0)
	{
		this->PlaySound(_weaponProperties->Dry_Sound, GetActorLocation());
		return;
	}

	if (SpawnProjectile())
	{
		if (_weaponProperties->MagazineSize != -1)
			CurrentAmmo = FMath::Max(0, CurrentAmmo - 1);

		Recoil();
		PlayShootFX();
		UAISense_Hearing::ReportNoiseEvent(
			GetWorld(),
			GetActorLocation(),
			1,
			this,
			_weaponProperties->NoiseRange,
			TEXT("Shooting"));
	}

	bReseted = _weaponProperties->bAllowAutoFire;
}

bool AFireWeapon::SpawnProjectile()
{
	if (!IsValid(Projectile))
		return false;

	// Define projectile spawn location
	FVector muzzleLocation = Muzzle->GetComponentLocation();
	FRotator muzzleRotation = Muzzle->GetComponentRotation();

	for (int i = 0; i < _weaponProperties->PelletsPerShoot; i++)
	{
		/*Define bullet throw direction*/
		FVector _start = muzzleLocation;
		FVector _end = (UKismetMathLibrary::GetForwardVector(AimPoint->GetComponentRotation()) * _weaponProperties->AttackRange * 100) +
					   _start;
		_end += GetBulletSpread();

		FHitResult _hit;
		UKismetSystemLibrary::SphereTraceSingle(this, _start, _end, _weaponProperties->AttackRadius,
												UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor *>(),
												EDrawDebugTrace::None, _hit, true);

		FVector _target = _hit.bBlockingHit ? _hit.Location : _hit.TraceEnd;
		float _len = (_target - _start).Size();
		float _t = _len / (_weaponProperties->BulletSpeed * 100);
		float _horizontalVelocity = (_len - (0.5f * (_t * _t) * (UPhysicsSettings::Get()->DefaultGravityZ / 2))) / _t;

		FVector _direction = UKismetMathLibrary::GetDirectionUnitVector(muzzleLocation, _target);

		/*Spawn and throw new bullet*/
		AProjectile *_bullet = GetWorld()->SpawnActor<AProjectile>(Projectile, muzzleLocation, muzzleRotation);
		if (!_bullet)
			return false;

		_bullet->SetOwner(GetOwner());
		_bullet->Init(_weaponProperties->ReboundProbability,
					  _weaponProperties->BaseDamage,
					  _weaponProperties->BulletHitImpulse);

		if (UProjectileMovementComponent *_projectileMovement = _bullet->GetProjectileMovement())
			_projectileMovement->Velocity = _direction * _horizontalVelocity;
	}

	return true;
}

void AFireWeapon::PlayShootFX()
{
	/*Activate muzzle flash component*/
	if (MuzzleFlash != nullptr)
		MuzzleFlash->Activate(true);

	if (ShellEject_System != nullptr)
		ShellEject_System->Activate(true);

	this->PlaySound(_weaponProperties->Shoot_Sound, GetActorLocation());
}

/* Update weapon mesh transform.
 * Move mesh back in defined direction with defined ammount
 * Use a Vector Curve and TimelineComponent to rotate the mesh and attached assets */
void AFireWeapon::Recoil()
{
	RecoilRotation_Timeline.PlayFromStart();
	RecoilTranslation_Timeline.PlayFromStart();

	// Add some recoil rotation to character controller by pitch and yaw axis
	float randPitch = -(UKismetMathLibrary::RandomFloatInRange(_weaponProperties->RecoilPitchMin, _weaponProperties->RecoilPitchMax));
	float randYaw = UKismetMathLibrary::RandomFloatInRange(_weaponProperties->RecoilYawMin, _weaponProperties->RecoilYawMax);

	if (TObjectPtr<ACharacter> _owner = Cast<ACharacter>(GetOwner()))
	{
		_owner->AddControllerPitchInput(randPitch * GetWorld()->GetDeltaSeconds());
		_owner->AddControllerYawInput(randYaw * GetWorld()->GetDeltaSeconds());
	}
	else
		logger.Error(FText::FromString(FString::Printf(TEXT("%s: Character owner is nullptr"), *GetName())));

	/*Add camera shake effect for recoil*/
	if (IsValid(_weaponProperties->RecoilCameraShake_Class) && _weaponProperties->bToogleCameraShake)
		UGameplayStatics::PlayWorldCameraShake(this, _weaponProperties->RecoilCameraShake_Class, GetActorLocation(), 350.f, 450.f);
}

void AFireWeapon::RotateMesh()
{
	float timelineValue = RecoilRotation_Timeline.GetPlaybackPosition();
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

void AFireWeapon::PlaySound(USoundBase *Sound, FVector Location)
{
	UWorld *world = GetWorld();
	if (world != nullptr && Sound != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, Location);
	}
}

bool AFireWeapon::CanBeReloaded_Implementation() const
{
	if (_weaponProperties)
	{
		return (!isReloading && CurrentAmmo < _weaponProperties->MagazineSize);
	}
	return false;
}

float AFireWeapon::GetCurrentAmmoPercent_Implementation()
{
	if (_weaponProperties)
		return static_cast<float>(CurrentAmmo) / static_cast<float>(_weaponProperties->MagazineSize);
	return 0.f;
}

bool AFireWeapon::CanADS_Implementation() const
{
	return _weaponProperties && _weaponProperties->bCanADS;
}

float AFireWeapon::GetADS_TurnRate_Implementation() const
{
	return _weaponProperties ? _weaponProperties->ADS_TurnRate : 1;
}

UAnimMontage *AFireWeapon::GetReloadMontage_Implementation()
{
	return _weaponProperties->Reload_Montage;
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

void AProjectile::Init(float ReboundProbability = 0.f, float DamageAmount = 1.f, float HitImpulse = 1.f)
{
	Rebound = ReboundProbability;
	Damage = DamageAmount;
	Impulse = HitImpulse;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (Tracer != nullptr)
		Tracer->Activate();

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::HitEvent);
}

UProjectileMovementComponent *AProjectile::GetProjectileMovement()
{
	return ProjectileMovement;
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
			GetOwner());
	}

	/*Apply impulse to hitted actor*/
	FVector _impulse = UKismetMathLibrary::GetRightVector(GetActorRotation()) * Impulse;
	if (OtherComp->IsAnySimulatingPhysics())
		OtherComp->AddImpulse(_impulse, Hit.BoneName);

	if (UPhysicalMaterial *_physMat = Hit.Component.Get()->BodyInstance.GetSimplePhysicalMaterial())
	{
		TEnumAsByte<EPhysicalSurface> _surface = _physMat->SurfaceType;
		if (FImpact *_impact = _impactData.Find(_surface))
		{
			if (_impact->impactSound)
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), _impact->impactSound, Hit.Location);

			if (_impact->impactEmmiter)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _impact->impactEmmiter, Hit.Location, Hit.ImpactNormal.Rotation());

			if (_impact->impactDecal)
				UGameplayStatics::SpawnDecalAttached(_impact->impactDecal, _impact->decalScale, OtherComp, NAME_None,
													 Hit.Location, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, _impact->decalLifeSpan);
		}
	}

	Tracer->Deactivate();
	ProjectileMovement->bShouldBounce = Cast<ACharacter>(OtherActor) ? 0 : UKismetMathLibrary::RandomBoolWithWeight(Rebound);
	if (!ProjectileMovement->bShouldBounce)
		Destroy();
}

#pragma endregion Projectile