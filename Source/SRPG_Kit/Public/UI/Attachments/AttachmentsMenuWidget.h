#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttachmentsMenuWidget.generated.h"

enum class EWeaponAttachmentTypes : uint8;

UCLASS()
class SRPG_KIT_API UAttachmentsMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativePreConstruct() override;

    virtual void TogglePanel(EWeaponAttachmentTypes panelType);

protected:
    UPROPERTY(EditDefaultsOnly, Category = MenuConfig)
    TSubclassOf<class UAttachmentsButton_Menu> _menuButtonTemplate;
    UPROPERTY(EditDefaultsOnly, Category = MenuConfig)
    TSubclassOf<class UAttachmentsPanel> _attachmentPanelTemplate;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox *buttonsList;
    UPROPERTY(meta = (BindWidget))
    class UOverlay* panelsContainer;

private:
    virtual void InitButtons();
    virtual void InitPanels();

    UPROPERTY()
    TMap<EWeaponAttachmentTypes, TObjectPtr<class UAttachmentsPanel>> _panels;
};