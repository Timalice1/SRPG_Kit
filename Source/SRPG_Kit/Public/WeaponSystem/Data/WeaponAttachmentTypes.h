#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponAttachmentTypes : uint8
{
    Scope,
    Muzzle,
    Magazine,

    Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EWeaponAttachmentTypes, EWeaponAttachmentTypes::Count);