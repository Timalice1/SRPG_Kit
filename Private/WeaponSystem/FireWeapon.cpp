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


#pragma region FireWeapon

AFireWeapon::AFireWeapon()
{
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
	this->KickBackAmount = 10.f;
	this->KickBack_Direction = FVector();
	this->MagazineSize = 30;
	this->RecoilInterpSpeed = 10.f;
	this->RecoilRecoverInterpSpeed = 10.f;

	this->MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>("MuzzleFlash");
	this->MuzzleFlash->AttachToComponent(this->Mesh, FAttachmentTransformRules::KeepRelativeTransform, "Muzzle");
}

void AFireWeapon::BeginPlay()
{
	Super::BeginPlay();

	/*Store default mesh relative transform values*/
	this->DefaultRotation = Mesh->GetRelativeRotation();
	this->DefaultTranslation = Mesh->GetRelativeLocation();

	this->CurrentAmmo = this->MagazineSize;
	AnimBP = Mesh->GetAnimInstance();

	/*Mesh rotation function to mesh rotation timeline*/
	if (this->RecoilMeshRotation_Curve) {
		FOnTimelineVector timelineCallback;
		timelineCallback.BindUFunction(this, FName("RotateMesh"));
		this->RecoilMeshRotaionTimeline.AddInterpVector(RecoilMeshRotation_Curve, timelineCallback);
	}

	if (MuzzleFlash != NULL && MuzzleFlash->IsActive())
		this->MuzzleFlash->Deactivate();
}

void AFireWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Start timeline
	RecoilMeshRotaionTimeline.TickTimeline(DeltaSeconds);

	//Restore mesh translation to default
	FVector defaultTranslation = UKismetMathLibrary::VInterpTo(Mesh->GetRelativeLocation(),
		DefaultTranslation,
		DeltaSeconds,
		RecoilRecoverInterpSpeed);
	Mesh->SetRelativeLocation(defaultTranslation);

	//Restore mesh rotation to default
	FRotator defaultRotation = UKismetMathLibrary::RInterpTo(Mesh->GetRelativeRotation(),
		DefaultRotation,
		DeltaSeconds,
		RecoilRecoverInterpSpeed);
	Mesh->SetRelativeRotation(defaultRotation);
}

void AFireWeapon::Attack() {
	//Break this function 
	//if weapon not reseted for new shoot
	if (!this->bReadyToShoot)
		return;

	//Shoot once if can, and then check 
	//if it allowing auto or burst firing
	//and if enough ammo
	AFireWeapon::Fire();
	if (bAllowAutoFire /* && (CurrentAmmo > 0)*/) {
		FTimerDelegate fireTimerDelegate;
		fireTimerDelegate.BindUFunction(this, "Fire");
		GetWorldTimerManager().SetTimer(FireTimer, fireTimerDelegate, FireRate, bAllowAutoFire);
	}

	controllerRotation_Default = CharacterOwner->GetControlRotation();
}

void AFireWeapon::StopAttack() {
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void AFireWeapon::Fire() {

	/*Do a empty shoot sound and finish this function*/
	if (!(CurrentAmmo > 0)) {
		this->StopAttack();
		return;
	}

	FVector muzzleLocation = Mesh->GetSocketLocation("Muzzle");
	FRotator muzzleRotation = Mesh->GetSocketRotation("Muzzle");
	FVector Direction = FVector();

	if (CharacterOwner->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) 
		Direction = ICombatInterface::Execute_GetAttackDirection(CharacterOwner, Range);
	else 
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "!!!'CombatInterface' IS REQUIRED ON A CHARACTER!!!");

	if (Projectile != NULL) {
		for (int i = 0; i < PelletsPerShoot; i++) {
			/*Define bullet throw direction*/
			FVector unitDirection =
				UKismetMathLibrary::GetDirectionUnitVector(muzzleLocation, Direction + Spread());

			/*Spawn and throw new bullet*/
			AProjectile* bullet = GetWorld()->SpawnActor<AProjectile>(Projectile, muzzleLocation, muzzleRotation);
			bullet->SetReboundProbability(ReboundProbability);
			bullet->SetDamageAmount(BaseDamage);
			bullet->Throw(unitDirection * Strenght);
		}
	}

	/*Activate muzzle flash particles*/
	if (MuzzleFlash != NULL)
		MuzzleFlash->Activate(true);

	AFireWeapon::Recoil();
	//CurrentAmmo--;

	/*Reset weapon for a new shoot after some delay*/
	bReadyToShoot = false;
	GetWorldTimerManager().SetTimer(ResetTimer, this, &AFireWeapon::ResetShoot, FireRate, false);
}

FVector AFireWeapon::Spread() {
	/* 
	* Add some bullet spread for weapon
	* Calculates random values from a given ranges
	*/

	float randYaw = UKismetMathLibrary::RandomFloatInRange(-MaxSpread, MaxSpread);
	float randPitch = UKismetMathLibrary::RandomFloatInRange(-MaxSpread, MaxSpread);

	return FVector(0, randYaw, randPitch);
}

void AFireWeapon::ResetShoot()
{
	bReadyToShoot = true;
	ResetTimer.Invalidate();
}

void AFireWeapon::Recoil()
{

	/*Update weapon mesh location (KickBack)*/
	FVector currentLocation = Mesh->GetRelativeLocation();
	FVector targetlLocation = currentLocation - (KickBack_Direction*KickBackAmount);
	FVector recoil = UKismetMathLibrary::VInterpTo(currentLocation, targetlLocation, GetWorld()->GetDeltaSeconds(), RecoilInterpSpeed);
	Mesh->SetRelativeLocation(recoil);

	/*Start the recoil rotation timeline (rotates a weapon mesh around pivot point)*/
	RecoilMeshRotaionTimeline.PlayFromStart();

	//Add some recoil rotation to character controller by pitch and yae axis
	float randPitch = -(UKismetMathLibrary::RandomFloatInRange(RecoilPitchMin, RecoilPitchMax));
	float randYaw = UKismetMathLibrary::RandomFloatInRange(RecoilYawMin, RecoilYawMax);

	CharacterOwner->AddControllerPitchInput(randPitch);
	CharacterOwner->AddControllerYawInput(randYaw);
	
	/*Add camera shake effect for recoil*/
	if (RecoilCameraShake_Class != NULL && bToogleCameraShake) {
		FVector cameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), RecoilCameraShake_Class, cameraLocation, 10.f, 10.f);
	}
}

void AFireWeapon::RotateMesh()
{
	//Get remaining time from timeline
	float timelineValue = RecoilMeshRotaionTimeline.GetPlaybackPosition();
	FRotator currentRotation = Mesh->GetRelativeRotation();

	//Add a values from recoil curve by timeline playback time
	//to current mesh rotation
	FVector curveValue = RecoilMeshRotation_Curve->GetVectorValue(timelineValue) * -1;
	FRotator recoilRotation = FRotator(currentRotation.Pitch + curveValue.X,
		currentRotation.Yaw + curveValue.Z,
		currentRotation.Roll + curveValue.Y);

	//Set the new mesh rotation
	Mesh->SetRelativeRotation(recoilRotation);
}

void AFireWeapon::Reload()
{
	CurrentAmmo = MagazineSize;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Yellow, "Reloaded");
}

#pragma endregion

#pragma region Projectile

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetLifeSpan(2.f);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	//ProjectileMesh->SetNotifyRigidBodyCollision(true);
	ProjectileMesh->SetGenerateOverlapEvents(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->bRotationFollowsVelocity = true;

	Tracer = CreateDefaultSubobject<UParticleSystemComponent>("Tracer");
	Tracer->AttachToComponent(ProjectileMesh, FAttachmentTransformRules::KeepRelativeTransform);

	Rebound = 0.f;
	ProjectileDamageMultiplyer = 1.f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::HitEvent);

	if (Tracer != NULL)
		Tracer->Activate();
}

void AProjectile::Throw(const FVector Direction)
{
	ProjectileMovement->Velocity = Direction;
}

void AProjectile::HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ProjectileMovement->bShouldBounce = UKismetMathLibrary::RandomBoolWithWeight(Rebound);
	Tracer->Deactivate();

	if (OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
		IDamageableInterface::Execute_TakeDamage(OtherActor, Damage * ProjectileDamageMultiplyer, Hit, ProjectileMovement->Velocity);

	GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, OtherComp->GetName());

	if(OtherComp->IsAnySimulatingPhysics())
		OtherComp->AddImpulse(ProjectileMovement->Velocity / 4, Hit.BoneName);
}

#pragma endregion