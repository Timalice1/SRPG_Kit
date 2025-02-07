#include "../Public/WeaponSystem/Loadout/LoadoutComponent.h"

ULoadoutComponent::ULoadoutComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void ULoadoutComponent::BeginPlay()
{
  Super::BeginPlay();
  return;
}

void ULoadoutComponent::AssignToSlot(const int32 slot, ABaseWeapon *Weapon, USceneComponent *AttachTo)
{
#if WITH_EDITOR
  if (Weapon == nullptr)
  {
    logger.Error(FText::FromString(FString::Printf(TEXT("%s: Weapon pin must have a connection, Node: AssignToSlot()"), *(GetOwner()->GetName()))));
    return;
  }

  if (slot > MaxSlots - 1 || slot < 0)
  {
    logger.Error(FText::FromString(FString::Printf(TEXT("%s: Slot %d is out of bounds of MaxSlots value, node: AssignToSlot"), *(GetOwner()->GetName()), slot)));
    return;
  }
#endif

  auto _weapon = Slots.Emplace(slot, Weapon);
  if (AttachTo != nullptr)
  {
    attachmentSlots.Emplace(slot, AttachTo);
    _weapon->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
  }

  return;
}

// Add here and to hideWeapon functions UAnimMontage parameters!!! <----
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
bool ULoadoutComponent::EquipWeapon(const int32 FromSlot, USceneComponent *AttachTo, FName AttachBoneName)
{
#if WITH_EDITOR
  if (!Slots.Contains(FromSlot))
  {
    logger.Error(FText::FromString(FString::Printf(TEXT("%s: Slot %d is not exists, Node:EquipWeapon()"), *(GetOwner()->GetName()), FromSlot)));
    return false;
  }

  if (Slots[FromSlot] == nullptr)
    return false;
#endif

  if (ActiveWeapon != nullptr)
    HideWeapon();

  // if (ActiveWeapon == Slots[FromSlot])
  // {
  //   HideWeapon();
  //   return true;
  // }

  ActiveWeapon = Slots[FromSlot];
  //ActiveWeapon->SetOwnerCharacter(Cast<ACharacter>(GetOwner()));
  ActiveWeapon->SetOwner(GetOwner());

  activeSlot = FromSlot;
  if (AttachTo != nullptr)
    ActiveWeapon->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachBoneName);

  OnSetActiveWeapon.Broadcast(ActiveWeapon);

  return true;
}

void ULoadoutComponent::HideWeapon()
{
  if (ActiveWeapon == nullptr)
    return;

  ActiveWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

  if (!attachmentSlots.IsEmpty() && attachmentSlots.Find(activeSlot))
    AssignToSlot(activeSlot, ActiveWeapon, attachmentSlots[activeSlot]);
  else 
    ActiveWeapon->SetActorLocation(FVector(0,0,0)); 

  ActiveWeapon->SetOwner(nullptr);
  ActiveWeapon = nullptr;
  activeSlot = -1;

  OnHideWeapon.Broadcast();

  return;
}

bool ULoadoutComponent::DropWeapon()
{
  if (ActiveWeapon != NULL && ActiveWeapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()) && activeSlot != -1)
  {
    IWeaponInterface::Execute_Drop(ActiveWeapon);
    ActiveWeapon->SetOwner(nullptr);
    ActiveWeapon = nullptr;
    Slots.Emplace(activeSlot);

    OnHideWeapon.Broadcast();

    return true;
  }
  return false;
}
