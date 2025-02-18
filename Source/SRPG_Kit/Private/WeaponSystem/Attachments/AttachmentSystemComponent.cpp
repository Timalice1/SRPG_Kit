#include "WeaponSystem/Attachments/AttachmentsSystemComponent.h"
#include "WeaponSystem/Firearm/FireWeapon.h"

UAttachmentSystemComponent::UAttachmentSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UAttachmentSystemComponent::BeginPlay()
{
    if (!Cast<AFireWeapon>(GetOwner()))
        return Super::BeginPlay();
    Super::BeginPlay();
    owningWeapon = Cast<AFireWeapon>(GetOwner());

    InitSlots();
}

void UAttachmentSystemComponent::InitSlots()
{
    for (FAttachmentSlot &slot : attachmentSlots)
    {
        if (owningWeapon->GetMesh()->DoesSocketExist(slot.SocketName))
            _activeSlots.Add(slot);
    }
}

void UAttachmentSystemComponent::InstallModule(const FName &SlotName, UStaticMesh *attachment)
{
    FAttachmentSlot *_targetSlot =
        _activeSlots.FindByPredicate([SlotName](const FAttachmentSlot &slot)
                                     { return slot.SlotName == SlotName; });

    if (!_targetSlot || !attachment)
        return; // Exit if slot don't exist or attachment is null

    if (_targetSlot->CurrentModule)
        RemoveModule(SlotName); // Remove attahcment if already installed

    UStaticMeshComponent *newComp = NewObject<UStaticMeshComponent>(owningWeapon, UStaticMeshComponent::StaticClass(), SlotName);
    if (!newComp)
        return;

    newComp->AttachToComponent(owningWeapon->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, _targetSlot->SocketName);
    newComp->SetStaticMesh(attachment);
    newComp->SetCollisionProfileName("NoCollision");
    newComp->RegisterComponent();

    _targetSlot->CurrentModule = newComp;
}

void UAttachmentSystemComponent::RemoveModule(const FName &SlotName)
{
    FAttachmentSlot *_targetSlot =
        _activeSlots.FindByPredicate([SlotName](const FAttachmentSlot &slot)
                                     { return slot.SlotName == SlotName; });
    if (!_targetSlot || !_targetSlot->CurrentModule)
        return;
    _targetSlot->CurrentModule->DestroyComponent();
    _targetSlot->CurrentModule = nullptr;
}
