#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeadEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterTakeDownEvent, FName, BoneName, bool, AddImpulse, FVector, HitImpulse);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterGetUpEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_KIT_API UCharacterStatsComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	bool bIsAlive;
	float CurrentHealth;

public:	

	UCharacterStatsComponent();

/*Component properties*/
protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Health", meta = (ClampMin = 0))
	float MaxHeath;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Health", meta = (ClampMin = "0", ClampMax = "1"))
	float CriticalHealthThressholdPercent;
	/*Character body life time after death*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character", meta = (ClampMin = "0"))
	float DestroyBodyTime;

/*Component events*/
protected:

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCharacterDeadEvent OnCharacterDead;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCharacterTakeDownEvent OnCharacterTakeDown;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCharacterGetUpEvent OnCharacterGetUp;

protected:

	UFUNCTION()
	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool GetIsAlive() const {
		return bIsAlive;
	}
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth() const {
		return CurrentHealth;
	}
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const {
		return CurrentHealth / MaxHeath;
	}
	UFUNCTION(BlueprintCallable, Category = "Health")
	bool TakeDamage(float Amount);
	/*Check if character current HP is under critical thresshold*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsCriticalHealth() const {
		return GetHealthPercent() < CriticalHealthThressholdPercent;
	};

};