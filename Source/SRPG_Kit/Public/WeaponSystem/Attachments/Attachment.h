#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem/Attachments/AttachmentSlot.h"
#include "Attachment.generated.h"

USTRUCT()
struct FAttachmentModuleData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BaseModuleParam)
    class UStaticMesh *Mesh;

    /*Child attachment slots*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BaseModuleParam)
    TSet<FAttachmentSlot> childSlots;

    // Override base weapon property by attachment modifiers
    virtual void OverrideWeaponData(struct FFireWeaponProperties &weaponData) {};
};

UCLASS(BlueprintType)
class SRPG_KIT_API UAttachmentModule : public UStaticMeshComponent
{
    GENERATED_BODY()

public:
    FAttachmentModuleData moduleData;

public:
    UAttachmentModule()
    {
        PrimaryComponentTick.bCanEverTick = false;
        PrimaryComponentTick.bStartWithTickEnabled = false;
    };
    virtual void BeginPlay() override {};
    virtual FAttachmentModuleData GetModuleData() { return moduleData; };
};