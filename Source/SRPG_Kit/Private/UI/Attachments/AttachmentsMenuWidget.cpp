#include "UI/Attachments/AttachmentsMenuWidget.h"
#include "UI/Attachments/AttachmentsButton_Menu.h"
#include "UI/Attachments/AttachmentsPanel.h"
#include "WeaponSystem/Data/WeaponAttachmentTypes.h"
#include "Components/VerticalBox.h"
#include "Components/Overlay.h"

void UAttachmentsMenuWidget::NativePreConstruct()
{
    InitButtons();
    InitPanels();
}

void UAttachmentsMenuWidget::InitButtons()
{
    if (!buttonsList || !_menuButtonTemplate)
        return;

    buttonsList->ClearChildren();
    for (EWeaponAttachmentTypes Type : TEnumRange<EWeaponAttachmentTypes>())
    {
        UAttachmentsButton_Menu *_button = NewObject<UAttachmentsButton_Menu>(this, _menuButtonTemplate);
        _button->ButtonLabel_Text = UEnum::GetDisplayValueAsText(Type);
        buttonsList->AddChild(_button);
        _button->SetMenuWidgetRef(this);
        _button->SetAttachmentType(Type);
    }
}

void UAttachmentsMenuWidget::InitPanels()
{
    if (!_attachmentPanelTemplate || !panelsContainer)
        return;

    panelsContainer->ClearChildren();
    for (EWeaponAttachmentTypes Type : TEnumRange<EWeaponAttachmentTypes>())
    {
        UAttachmentsPanel *_panel = NewObject<UAttachmentsPanel>(this, _attachmentPanelTemplate);
        _panel->PanelLabel_Text = UEnum::GetDisplayValueAsText(Type);
        panelsContainer->AddChild(_panel);
        _panel->SetPanelType(Type);
        _panel->SetVisibility(ESlateVisibility::Collapsed);
        _panels.Add(Type, _panel);
    }
}

void UAttachmentsMenuWidget::TogglePanel(EWeaponAttachmentTypes panelType)
{
    for (auto &_panel : _panels)
        _panel.Value->SetVisibility(ESlateVisibility::Collapsed);

    if(TObjectPtr<UAttachmentsPanel> *_panel = _panels.Find(panelType))
        (*_panel)->SetVisibility(ESlateVisibility::Visible);
}
