#include "UI/Attachments/AttachmentsMenuWidget.h"
#include "UI/Attachments/AttachmentsMenuButton.h"
#include "UI/Attachments/AttachmentsPanel.h"
#include "WeaponSystem\Attachments\AttachmentsSystemComponent.h"
#include "WeaponSystem/Attachments/AttachmentSlot.h"
#include "WeaponSystem/Firearm/FireWeapon.h"
#include "Components/VerticalBox.h"
#include "Components/Overlay.h"

void UAttachmentsMenuWidget::NativeConstruct()
{
    if (!weaponRef)
        return Super::NativeConstruct();

    UActorComponent *_tmp = weaponRef->GetComponentByClass(UAttachmentSystemComponent::StaticClass());
    attachmentsComponentRef = Cast<UAttachmentSystemComponent>(_tmp);
    if (!attachmentsComponentRef)
        return Super::NativeConstruct();

    if (!panelTemplate || !slotButtonTemplate)
        return Super::NativeConstruct();

    // Clear containers at start
    slotsList->ClearChildren();
    panelsContainer->ClearChildren();
    _buttonToPanelMap.Empty();

    for (FAttachmentSlot &activeSlot : attachmentsComponentRef->GetActiveSlots())
    {
        FText _label = FText::FromName(activeSlot.SlotName);

        UAttachmentsMenuButton *newSlotButton = NewObject<UAttachmentsMenuButton>(this, slotButtonTemplate);
        UAttachmentsPanel *newPanel = NewObject<UAttachmentsPanel>(this, panelTemplate);
        if (!newSlotButton || !newPanel)
            continue;

        newSlotButton->buttonLabelText = _label;
        newPanel->panelLabel_Text = _label;
        newPanel->SetVisibility(ESlateVisibility::Collapsed);

        slotsList->AddChild(newSlotButton);
        panelsContainer->AddChild(newPanel);

        _buttonToPanelMap.Add(newSlotButton, newPanel);
        newSlotButton->OnClick.AddDynamic(this, &ThisClass::TogglePanel);
    }

    Super::NativeConstruct();
}

void UAttachmentsMenuWidget::TogglePanel(UAttachmentsMenuButton *slotButton)
{
    // Hide current active panel
    if (currentActivePanel)
        currentActivePanel->SetVisibility(ESlateVisibility::Collapsed);

    UAttachmentsPanel *targetPanel = *_buttonToPanelMap.Find(slotButton);
    if (!targetPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("AttachmentsMenuWidget::TooglePanel - Requested panel not found"));
        return;
    }

    // Just close active panel if requested same panel
    if (targetPanel == currentActivePanel)
    {
        currentActivePanel = nullptr;
        return;
    }

    currentActivePanel = targetPanel;
    currentActivePanel->SetVisibility(ESlateVisibility::Visible);
}
