#pragma once

#include "CoreMinimal.h"
#include "Attachment.generated.h"

UENUM(BlueprintType)
enum class EAttachmentModuleType : uint8
{
    Scope,
    Optics,
    Muzzle,
    Magazine,
    Mount,

    Count UMETA(Hidden)
};

USTRUCT()
struct FAttachmentModule : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BaseModuleParam)
    class UStaticMesh *Mesh;
    
    // Override base weapon property by attachment modifiers
    virtual void OverrideWeaponData(struct FFireWeaponProperties &weaponData) {};
};
