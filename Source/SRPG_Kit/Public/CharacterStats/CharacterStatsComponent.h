#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeadEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChangedEvent);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Health", meta = (ClampMin = "0", ClampMax = "1", Units = "Percent"))
	float CriticalHealthThressholdPercent;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
	bool bIsImmortal;
	/*Character body life time after death*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character", meta = (ClampMin = "0"))
	float DestroyBodyTime;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
	float TakeDownTime;
	UPROPERTY(BlueprintReadWrite, Category = "Damage")
	bool bStuned;

public:
	/*Calls on character dead*/
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCharacterDeadEvent OnCharacterDead;

	/*Calls whenewer character health is changed (increased or decreased)*/
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnHealthChangedEvent OnHealtChanged;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "CharacterStats|Health")
	bool GetIsAlive() const
	{
		return bIsAlive;
	}

	UFUNCTION(BlueprintCallable, Category = "CharacterStats|Health")
	void SetIsAlive(bool IsAlive)
	{
		bIsAlive = IsAlive;
	}

	UFUNCTION(BlueprintCallable, Category = "CharacterStats|Health")
	float GetCurrentHealth() const
	{
		return CurrentHealth;
	}
	UFUNCTION(BlueprintCallable, Category = "CharacterStats|Health")
	float GetHealthPercent() const
	{
		return CurrentHealth / MaxHeath;
	}
	UFUNCTION(BlueprintCallable, Category = "CharacterStats|Health")
	bool TakeDamage(float Amount);
	/*Check if character current HP is under critical thresshold*/
	UFUNCTION(BlueprintCallable, Category = "CharacterStats|Health")
	bool IsCriticalHealth() const
	{
		return GetHealthPercent() < CriticalHealthThressholdPercent;
	};

	UFUNCTION(BlueprintCallable, Category = "CharacterStats|Health")
	void Heal(float Amount); 

	UFUNCTION(BlueprintCallable, Category = "Health|Damageable")
	float CalculateDamage(const float BaseDamage, FHitResult HitResult);
};
