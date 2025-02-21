#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttachmentsPanel.generated.h"

UCLASS()
class SRPG_KIT_API UAttachmentsPanel : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(EditDefaultsOnly, Category = PanelConfig)
    FText panelLabel_Text = FText::FromString("AttachmentsPanel");

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> panelLabel;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UVerticalBox> attachmentsList;

    UPROPERTY(EditDefaultsOnly, Category = MenuConfig)
    TSubclassOf<class UAttachmentsMenuButton> slotButtonTemplate;
};