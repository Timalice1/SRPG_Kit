#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponSystem/Attachments/AttachmentSlot.h"
#include "AttachmentsSystemComponent.generated.h"

UCLASS()
class SRPG_KIT_API UAttachmentSystemComponent : public UActorComponent
{
    GENERATED_BODY()

    /* Init default weapon slots */
    virtual void InitSlots();
    /* Add a new slot*/
    virtual void AddSlot(FAttachmentSlot &slot, USceneComponent *parent);
    virtual FAttachmentSlot *FindSlotByName(const FName &SlotName);

public:
    UAttachmentSystemComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    TArray<FAttachmentSlot> GetActiveSlots() { return _activeSlots; };

public:
    /**
     * @brief Install new module to required slot
     * @param SlotName required slot name
     * @param moduleData new module data to install
     */
    UFUNCTION(BlueprintCallable, Category = AttachmentSystemComponent)
    virtual void InstallModule(const FName &SlotName, const struct FAttachmentModuleData &moduleData);

    /**
     * @brief Remove current active module from required slot
     * @param SlotName Required slot name
     */
    UFUNCTION(BlueprintCallable, Category = AttachmentSystemComponent)
    virtual void RemoveModule(const FName &SlotName = FName("ModuleSlot"));

protected:
    /* Default weapon attachment slots (persistant)*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AttachmentsSystem")
    TSet<FAttachmentSlot> attachmentSlots;

    /* Active attachment slots, includes child slots from attachments*/
    TArray<FAttachmentSlot> _activeSlots;

private: // References
    UPROPERTY()
    TObjectPtr<class AFireWeapon> owningWeapon = nullptr;
};