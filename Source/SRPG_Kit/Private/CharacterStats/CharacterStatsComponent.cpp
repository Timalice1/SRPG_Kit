#include "CharacterStats/CharacterStatsComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UCharacterStatsComponent::UCharacterStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bIsAlive = true;

	MaxHeath = 100.f;
	CriticalHealthThressholdPercent = .2f;
	TakeDownTime = 5.f;
}

void UCharacterStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHeath;
}

bool UCharacterStatsComponent::TakeDamage(float Amount)
{
	// Exit if character is already dead or if character is invincible
	if (!bIsAlive || bIsImmortal)
		return false;

	CurrentHealth = FMath::Max(0, CurrentHealth - Amount);

	OnHealtChanged.Broadcast();
	return true;
}

float UCharacterStatsComponent::CalculateDamage(const float BaseDAmage, FHitResult HitResult) 
{
	return 0.f;
}

void UCharacterStatsComponent::Heal(float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth+Amount, 0, MaxHeath);
}