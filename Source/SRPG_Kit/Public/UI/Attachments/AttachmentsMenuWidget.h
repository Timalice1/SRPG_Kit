#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttachmentsMenuWidget.generated.h"

UCLASS()
class SRPG_KIT_API UAttachmentsMenuWidget : public UUserWidget
{
    GENERATED_BODY()

private:
    UPROPERTY()
    TObjectPtr<class UAttachmentSystemComponent> attachmentsComponentRef;
    UPROPERTY()
    TMap<class UAttachmentsMenuButton *, class UAttachmentsPanel *> _buttonToPanelMap;
    UPROPERTY()
    class UAttachmentsPanel* currentActivePanel = nullptr;

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UVerticalBox> slotsList;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UOverlay> panelsContainer;

protected:
    UPROPERTY(EditDefaultsOnly, Category = MenuConfig)
    TSubclassOf<class UAttachmentsMenuButton> slotButtonTemplate;
    UPROPERTY(EditDefaultsOnly, Category = MenuConfig)
    TSubclassOf<class UAttachmentsPanel> panelTemplate;

    UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true))
    TObjectPtr<class AFireWeapon> weaponRef;

protected:
    UFUNCTION()
    virtual void TogglePanel(class UAttachmentsMenuButton* slotButton);
};