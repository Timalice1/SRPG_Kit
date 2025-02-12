#pragma once

#include "CoreMinimal.h"
#include "Attachment.generated.h"

USTRUCT(BlueprintType)
struct FAttachmentConfig : public FTableRowBase
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = BaseConfig)
    TObjectPtr<class UStaticMesh> Mesh;
};

UCLASS(MinimalAPI, BlueprintType, Blueprintable)
class AAttachment : public AActor
{
    GENERATED_BODY()

public:
    AAttachment() {};
};