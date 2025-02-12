#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttachmentsPanel.generated.h"

enum class EWeaponAttachmentTypes : uint8;

UCLASS()
class SRPG_KIT_API UAttachmentsPanel : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override { Super::NativeConstruct(); };
   
    virtual void SetPanelType(EWeaponAttachmentTypes panelType) { _panelType = panelType; };
    virtual EWeaponAttachmentTypes GetPanelType() { return _panelType; };

    UPROPERTY()
    FText PanelLabel_Text;

private:
    UPROPERTY()
    EWeaponAttachmentTypes _panelType;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> panelLabel;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UVerticalBox> attachmentsList;
};