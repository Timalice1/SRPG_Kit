#pragma once

#include "CoreMinimal.h"
#include "AttachmentSlot.generated.h"

USTRUCT(BlueprintType)
struct FAttachmentSlot
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AttachmentSlot)
    FName SocketName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AttachmentSlot)
    FName SlotName = FName("AttachmentSlot");

    UPROPERTY()
    class UStaticMeshComponent *CurrentModule;

    friend uint32 GetTypeHash(const FAttachmentSlot &Other)
    {
        return FCrc::MemCrc32(&Other, sizeof(FAttachmentSlot));
    }
};