#pragma once

#include "AttachmentsButton_Base.h"
#include "AttachmentsButton_Menu.generated.h"

enum class EWeaponAttachmentTypes : uint8;

UCLASS()
class SRPG_KIT_API UAttachmentsButton_Menu : public UAttachmentsButton_Base
{
    GENERATED_BODY()

protected:
    virtual void OnClick() override;

public:
    virtual void SetMenuWidgetRef(class UAttachmentsMenuWidget *menuWidgetRef);
    virtual void SetAttachmentType(EWeaponAttachmentTypes newType);

private:
    UPROPERTY()
    TObjectPtr<class UAttachmentsMenuWidget> _menuWidgetRef;
    UPROPERTY()
    EWeaponAttachmentTypes _attachmentType;
};