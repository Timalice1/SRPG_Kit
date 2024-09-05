#include "../Public/WeaponSystem/Loadout/LoadoutComponent.h"

ULoadoutComponent::ULoadoutComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void ULoadoutComponent::BeginPlay()
{
  Super::BeginPlay();
  return;
}

void ULoadoutComponent::AssignToSlot(const int32 slot, ABaseWeapon *Weapon)
{
  if (Weapon == nullptr)
  {
    logger.Error(FText::FromString(FString::Printf(TEXT("%s: Weapon pin mus have a connection, Node: AssignToSlot()"), *(GetOwner()->GetName()))));
    return;
  }

  if (slot > MaxSlots - 1 || slot < 0)
  {
    logger.Error(FText::FromString(FString::Printf(TEXT("%s: Slot %d is out of bounds of MaxSlots value, node: AssignToSlot"), *(GetOwner()->GetName()), slot)));
    return;
  }

  Slots.Emplace(slot, Weapon);
  return;
}

bool ULoadoutComponent::EquipWeapon(const int32 FromSlot, USceneComponent *AttachTo, const FName AttachBoneName)
{
  if (!Slots.Contains(FromSlot))
  {
    logger.Error(FText::FromString(FString::Printf(TEXT("%s: Slot %d is not exists, Node:EquipWeapon()"), *(GetOwner()->GetName()), FromSlot)));
    GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, FString::Printf(TEXT("Slot %d is not exist"), FromSlot));
    return false;
  }

  if (Slots[FromSlot] == nullptr)
  {
    GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, FString::Printf(TEXT("Slot %d is empty"), FromSlot));
    return false;
  }

  ActiveWeapon = Slots[FromSlot];
  if (AttachTo != nullptr)
    ActiveWeapon->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachBoneName);

  return true;
}

void ULoadoutComponent::HideWeapon()
{
  ActiveWeapon = NULL;
  return;
}

bool ULoadoutComponent::DropWeapon()
{
  if (ActiveWeapon != NULL && ActiveWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
  {
    IWeaponInterface::Execute_Drop(ActiveWeapon);
    ActiveWeapon = nullptr;
    return true;
  }

  GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, FString::Printf(TEXT("No active weapon found")));
  return false;
}
