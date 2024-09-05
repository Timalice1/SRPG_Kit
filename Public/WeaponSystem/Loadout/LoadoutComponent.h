#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "../BaseWeapon.h"
#include "Logging/MessageLog.h"
#include "LoadoutComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SRPG_KIT_API ULoadoutComponent : public UActorComponent
{
  GENERATED_BODY()

  FMessageLog logger = FMessageLog(FName("PIE"));

public:
  ULoadoutComponent();

protected:
  /*Current weapon that character holds*/
  UPROPERTY()
  ABaseWeapon *ActiveWeapon;

  UPROPERTY(EditDefaultsOnly, Category = "Loadout|Slots", meta = (ClampMin = 1))
  int32 MaxSlots = 1;

  /*Map of loadout slots*/
  UPROPERTY(BlueprintReadOnly, Category = "Loadout|Slots")
  TMap<int32, ABaseWeapon *> Slots;

protected:
  UFUNCTION()
  void BeginPlay() override;

  UFUNCTION(BlueprintCallable, Category = "Loadout")
  ABaseWeapon *GetActiveWeapon() const { return ActiveWeapon; };

  UFUNCTION(BlueprintCallable, Category = "Loadout|Slots")
  void AssignToSlot(const int32 slot, ABaseWeapon *Weapon);

  UFUNCTION(BlueprintCallable, Category = "Loadout|Slots")
  bool EquipWeapon(const int32 FromSlot, USceneComponent *AttachTo, const FName AttachBoneName);

  UFUNCTION(BlueprintCallable, Category = "Loadout|ActiveWeapon")
  void HideWeapon();

  UFUNCTION(BlueprintCallable, Category = "Loadout|ActiveWeapon")
  bool DropWeapon();
};