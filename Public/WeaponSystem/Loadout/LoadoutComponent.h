#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "../BaseWeapon.h"
#include "Logging/MessageLog.h"
#include "../CombatInterface.h"
#include "LoadoutComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetActiveWeaponEvent, class ABaseWeapon *, ActiveWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideWeaponEvent);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SRPG_KIT_API ULoadoutComponent : public UActorComponent
{
  GENERATED_BODY()

  FMessageLog logger = FMessageLog(FName("PIE"));

  UPROPERTY()
  int32 activeSlot = -1;

  UPROPERTY()
  TMap<int32, USceneComponent *> attachmentSlots;

public:
  ULoadoutComponent();

  /*Calls when equip a weapon*/
  UPROPERTY(BlueprintAssignable, BlueprintCallable)
  FOnSetActiveWeaponEvent OnSetActiveWeapon;

  /*Calls when unequip / drop active weapon*/
  UPROPERTY(BlueprintAssignable, BlueprintCallable)
  FOnHideWeaponEvent OnHideWeapon;

protected:
  /*Current weapon that character holds*/
  UPROPERTY()
  ABaseWeapon *ActiveWeapon;

  UPROPERTY(EditDefaultsOnly, Category = "Loadout|Slots", meta = (ClampMin = 1))
  int32 MaxSlots = 1;

  /*Map of loadout slots*/
  UPROPERTY()
  TMap<int32, ABaseWeapon *> Slots;

protected:
  UFUNCTION()
  void BeginPlay() override;

  UFUNCTION(BlueprintCallable, Category = "Loadout")
  ABaseWeapon *GetActiveWeapon() const { return ActiveWeapon; };

  /**
   * Returns a map of loadout slots and assigned weapons
   */
  UFUNCTION(BlueprintCallable, Category = "Loadout|Slots")
  TMap<int32, ABaseWeapon *> GetSlots() const
  {
    return Slots;
  }

  /**
   * Assigning current weapon to selected slot number
   * Weapon object reference cannot be nullptr and slot number must be in range 0 - MaxSlots
   * @param AttachTo Represents scene component, where weapon needs to be attached (ex. on character mesh).
   * Can be null, then weapon are not be attached to character
   */
  UFUNCTION(BlueprintCallable, Category = "Loadout|Slots")
  void AssignToSlot(const int32 slot, ABaseWeapon *Weapon, USceneComponent *AttachTo);

  /**
   * Equip weapon from selected slot.
   * It will make weapon from slot (if slot is not empty) active,
   * and attach to given scene component (ex. in character hand)
   */
  UFUNCTION(BlueprintCallable, Category = "Loadout|Slots")
  bool EquipWeapon(const int32 FromSlot, USceneComponent *AttachTo, const FName AttachBoneName);

  /**
   * Attach active weapon back to holster slot
   */
  UFUNCTION(BlueprintCallable, Category = "Loadout|ActiveWeapon")
  void HideWeapon();

  UFUNCTION(BlueprintCallable, Category = "Loadout|ActiveWeapon")
  bool DropWeapon();
};