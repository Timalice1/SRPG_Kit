#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterControllerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCharacterControllerInterface : public UInterface
{
    GENERATED_BODY()
};

class SRPG_KIT_API ICharacterControllerInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage|Callback")
    void OnKillAnyCharacter(class ACharacter *KilledCharacter);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage|Callback")
    void OnDamageAnyCharacter(class ACharacter *DamagedCharacter);
};
