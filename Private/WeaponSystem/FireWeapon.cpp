#include "WeaponSystem/FireWeapon.h"
#include "Curves/CurveVector.h"
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include <DrawDebugHelpers.h>
#include "Kismet/GameplayStatics.h"
#include <Particles/ParticleSystemComponent.h>
#include "WeaponSystem/CombatInterface.h"


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

	/*Binding Timeline component to target function and vector curve*/
	if (this->RecoilRotation_Curve) {
		FOnTimelineVector timelineCallback;
		timelineCallback.BindUFunction(this, FName("RotateMesh"));
		this->RecoilRotaionTimeline.AddInterpVector(RecoilRotation_Curve, timelineCallback);
	}

	if (MuzzleFlash != NULL && MuzzleFlash->IsActive())
		this->MuzzleFlash->Deactivate();
}

void AFireWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Start timeline
	RecoilRotaionTimeline.TickTimeline(DeltaSeconds);

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
}

void AFireWeapon::StopAttack() {
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void AFireWeapon::Fire() {

	/*Do a empty shoot sound and finish this function*/
	if (!(CurrentAmmo > 0)) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Reload Required");
		this->StopAttack();
		return;
	}

	FVector muzzleLocation = Mesh->GetSocketLocation("Muzzle");
	FRotator muzzleRotation = Mesh->GetSocketRotation("Muzzle");
	FVector Direction = FVector();

	if (CharacterOwner->GetClass()->ImplementsInterface(UCombatInterface::StaticClass())) {
		Direction = ICombatInterface::Execute_GetAttackDirection(CharacterOwner, Range);
	}

	if (BulletClass != NULL) {
		for (int i = 0; i < PelletsPerShoot; i++) {
			/*Define bullet throw direction*/
			FVector unitDirection =
				UKismetMathLibrary::GetDirectionUnitVector(muzzleLocation, Direction + Spread());

			/*Spawn and throw new bullet*/
			GetWorld()->SpawnActor<ABullet>(BulletClass.Get(),
				muzzleLocation, muzzleRotation)
				->Throw(unitDirection * Strenght);
		}
	}

	/*Activate muzzle flash particles*/
	if (MuzzleFlash != NULL)
		MuzzleFlash->Activate(true);

	AFireWeapon::Recoil();
	//CurrentAmmo--;

	/*Slide a gun shelleject bone
	(in weapon anim blueprint)*/
	//if (AnimBP != NULL && AnimBP->GetClass()->ImplementsInterface(UWeaponABPInterface::StaticClass()))
		//Cast<IWeaponABPInterface>(AnimBP)->Execute_Slide(AnimBP);

	/*Reset weapon for a new shoot after some delay*/
	bReadyToShoot = false;
	GetWorldTimerManager().SetTimer(ResetTimer, this, &AFireWeapon::ResetShoot, FireRate, false);
}

FVector AFireWeapon::Spread() {

	float randYaw = UKismetMathLibrary::RandomFloatInRange(-MaxSpread, MaxSpread);
	float randPitch = UKismetMathLibrary::RandomFloatInRange(-MaxSpread, MaxSpread);

	return FVector(0, randYaw, randPitch);
}

void AFireWeapon::ResetShoot()
{
	//MuzzleFlash->Deactivate();
	bReadyToShoot = true;
	ResetTimer.Invalidate();
}

void AFireWeapon::Recoil()
{

	FVector currentLocation = Mesh->GetRelativeLocation();
	FVector targetlLocation = currentLocation - (KickBack_Direction*KickBackAmount);
	FVector recoil = UKismetMathLibrary::VInterpTo(currentLocation, targetlLocation, GetWorld()->GetDeltaSeconds(), RecoilInterpSpeed);
	Mesh->SetRelativeLocation(recoil);

	/*Start the recoil rotation timeline*/
	RecoilRotaionTimeline.PlayFromStart();

	float randYaw = UKismetMathLibrary::RandomFloatInRange(RecoilYawMin, RecoilYawMax);
	float randPitch = UKismetMathLibrary::RandomFloatInRange(RecoilPitchMin, RecoilPitchMax);
	/*CharacterOwner->AddControllerPitchInput(-randPitch);
	CharacterOwner->AddControllerYawInput(randYaw);*/
	
	/*Add camera shake effect for recoil*/
	if (RecoilCameraShake_Class != NULL && bToogleCameraShake)
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), RecoilCameraShake_Class, CharacterOwner->GetActorLocation(), 200.f, 200.f);

}

void AFireWeapon::RotateMesh()
{
	//Get remaining time from timeline
	float timelineValue = RecoilRotaionTimeline.GetPlaybackPosition();
	FRotator currentRotation = Mesh->GetRelativeRotation();

	//Add a values from recoil curve by timeline playback time
	//to current mesh rotation
	FVector curveValue = RecoilRotation_Curve->GetVectorValue(timelineValue) * -1;
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


/*
Default bullet logic
*/

ABullet::ABullet()
{
	Root = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = Root;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	BulletMesh->SetNotifyRigidBodyCollision(true);
	BulletMesh->SetCollisionProfileName("Bullet");

	BulletMesh->SetupAttachment(Root);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->bRotationFollowsVelocity = true;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	BulletMesh->OnComponentHit.AddDynamic(this, &ABullet::HitEvent);
}

void ABullet::Throw(const FVector Direction)
{
	ProjectileMovement->Velocity = Direction;
}

void ABullet::HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL)) {
		DrawDebugSphere(GetWorld(), Hit.Location, 10.f, 10, FColor::Red, false, 5.f, 0, .1f);
		Destroy();
	}
}
