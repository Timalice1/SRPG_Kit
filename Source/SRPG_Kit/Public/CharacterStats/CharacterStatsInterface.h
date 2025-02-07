#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterStatsInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCharacterStatsInterface : public UInterface
{
	GENERATED_BODY()
};

class SRPG_KIT_API ICharacterStatsInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stamina")
	void RefreshStamina(float Amount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Stamina")
	void DrainStamina(float Amount) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stamina")
	float GetStaminaPercent() const;
};
