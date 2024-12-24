#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExplosiveComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SRPG_KIT_API UExplosiveComponent : public UActorComponent
{

	GENERATED_BODY()

	UWorld *world;
	FVector epicenter;

	UPROPERTY()
	class AActor *damageInstigator = nullptr;

public:
	UExplosiveComponent();

protected:
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugSphere;

	UPROPERTY(EditDefaultsOnly, Category = "ExplosionComponent")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent")
	TArray<AActor *> IgnoreActors;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent", meta = (ClampMin = .01f))
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent")
	float DestroyTime = 1.f;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent")
	float ExplosionDamage;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent")
	float ExplosionImpulse;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent")
	float ExplosionNoiseRange = ExplosionRadius * 2.f;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent|FX")
	class UParticleSystem *ExplosionEmmiter;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent|FX")
	TObjectPtr<class USoundCue> ExplosionSound;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent|FX", meta = (ClampMin = 0.f))
	float ExplosionEmmiterScale;
	UPROPERTY(EditAnywhere, Category = "ExplosionComponent|FX")
	TSubclassOf<UCameraShakeBase> ExplosionCameraShake_Class;

public:
	UFUNCTION(BlueprintCallable, Category = "ExplosionComponent")
	void Explode(float Damage, class AActor *DamageCauser);

private:
	void Explode_Implementation();

	virtual void DestroyOwner();
};
