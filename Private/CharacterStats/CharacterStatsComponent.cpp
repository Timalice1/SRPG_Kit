#include "CharacterStats/CharacterStatsComponent.h"

UCharacterStatsComponent::UCharacterStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsAlive = true;

	MaxHeath = 100.f;
	CriticalHealthThressholdPercent = .2f;

}

void UCharacterStatsComponent::BeginPlay()
{
	CurrentHealth = MaxHeath;
}

bool UCharacterStatsComponent::TakeDamage(float Amount)
{
	//Exit if character is already dead
	if (!bIsAlive)
		return false;

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("%.2f"), Amount));

	CurrentHealth -= Amount;
	if (CurrentHealth <= 0)
		bIsAlive = false;

	return true;
}