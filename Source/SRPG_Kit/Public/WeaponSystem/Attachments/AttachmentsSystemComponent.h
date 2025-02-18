#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponSystem/Attachments/AttachmentSlot.h"
#include "AttachmentsSystemComponent.generated.h"

UCLASS()
class SRPG_KIT_API UAttachmentSystemComponent : public UActorComponent
{
    GENERATED_BODY()

    virtual void InitSlots();

public:
    UAttachmentSystemComponent();
    virtual void BeginPlay();

    UFUNCTION(BlueprintCallable, Category = AttachmentSystemComponent)
    virtual void InstallModule(const FName &SlotName, UStaticMesh *attachment);
    UFUNCTION(BlueprintCallable, Category = AttachmentSystemComponent)
    virtual void RemoveModule(const FName &SlotName);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AttachmentsSystem")
    TSet<FAttachmentSlot> attachmentSlots;
    TArray<FAttachmentSlot> _activeSlots;

private: // References
    UPROPERTY()
    TObjectPtr<class AFireWeapon> owningWeapon = nullptr;
};