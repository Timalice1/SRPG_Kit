#include "CharacterStats/CharacterStatsComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UCharacterStatsComponent::UCharacterStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsAlive = true;

	MaxHeath = 100.f;
	CriticalHealthThressholdPercent = .2f;
	TakeDownTime = 5.f;
}

void UCharacterStatsComponent::BeginPlay()
{
	CurrentHealth = MaxHeath;
}

bool UCharacterStatsComponent::TakeDamage(float Amount)
{
	// Exit if character is already dead or if character is invincible
	if (!bIsAlive || bIsImmortal)
		return false;

	CurrentHealth = FMath::Max(0, CurrentHealth - Amount);
	return true;
}