#include "WeaponSystem/Attachments/AttachmentsSystemComponent.h"
#include "WeaponSystem/Firearm/FireWeapon.h"
#include "WeaponSystem/Attachments/Attachment.h"
#include "Components/StaticMeshComponent.h"

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
            AddSlot(slot, owningWeapon->GetMesh());
    }
}

void UAttachmentSystemComponent::AddSlot(FAttachmentSlot &slot, USceneComponent *parent)
{
    slot.parent = parent;
    _activeSlots.Add(slot);
}

FAttachmentSlot *UAttachmentSystemComponent::FindSlotByName(const FName &SlotName)
{
    FAttachmentSlot *_targetSlot =
        _activeSlots.FindByPredicate([SlotName](const FAttachmentSlot &slot)
                                     { return slot.SlotName == SlotName; });
    return _targetSlot;
}

void UAttachmentSystemComponent::InstallModule(const FName &SlotName, const FAttachmentModuleData &moduleData)
{
    /* Find slot by name */
    FAttachmentSlot *_targetSlot = FindSlotByName(SlotName);
    if (!_targetSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("Requested slot [%s] does not exist"), *SlotName.ToString());
        return;
    }

    // Remove existing module and child slots before adding new one
    RemoveModule(SlotName);

    UStaticMeshComponent *newComp = NewObject<UStaticMeshComponent>(owningWeapon, UStaticMeshComponent::StaticClass(), SlotName);
    if (!newComp)
        return;
    newComp->AttachToComponent(_targetSlot->parent, FAttachmentTransformRules::SnapToTargetIncludingScale, _targetSlot->SocketName);
    newComp->SetStaticMesh(moduleData.Mesh);
    newComp->SetCollisionProfileName("NoCollision");
    newComp->RegisterComponent();
    _targetSlot->CurrentModule = newComp;

    // Add child slots to global active slots list
    if (!moduleData.childSlots.IsEmpty())
    {
        for (FAttachmentSlot &slot : moduleData.childSlots.Array())
            AddSlot(slot, newComp);
    }
}

void UAttachmentSystemComponent::RemoveModule(const FName &SlotName)
{
    // Find requested slot
    FAttachmentSlot *_targetSlot = FindSlotByName(SlotName);
    if (!_targetSlot || !_targetSlot->CurrentModule)
    {
        UE_LOG(LogTemp, Warning, TEXT("Requested slot [%s] does not exist, or already empty"), *SlotName.ToString());
        return;
    }

    TArray<FAttachmentSlot> childSlots =
        _activeSlots.FilterByPredicate([_targetSlot](const FAttachmentSlot &slot)
                                       { return slot.parent == _targetSlot->CurrentModule; });
    // Recursively remove child modules and slots
    for (FAttachmentSlot &childSlot : childSlots)
    {
        RemoveModule(childSlot.SlotName);
        const int32 index = _activeSlots.IndexOfByKey(childSlot);
        _activeSlots.RemoveAtSwap(index, 1, EAllowShrinking::No);
    }
    _activeSlots.Shrink();

    _targetSlot->CurrentModule->DestroyComponent(true);
    _targetSlot->CurrentModule = nullptr;
}
