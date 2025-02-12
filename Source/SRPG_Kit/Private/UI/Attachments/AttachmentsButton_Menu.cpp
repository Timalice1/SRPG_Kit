#include "UI/Attachments/AttachmentsButton_Menu.h"
#include "UI/Attachments/AttachmentsMenuWidget.h"

void UAttachmentsButton_Menu::OnClick()
{
    Super::OnClick();
    if (_menuWidgetRef)
        _menuWidgetRef->TogglePanel(_attachmentType);
}

void UAttachmentsButton_Menu::SetMenuWidgetRef(UAttachmentsMenuWidget *menuWidgetRef)
{
    _menuWidgetRef = menuWidgetRef;
}

void UAttachmentsButton_Menu::SetAttachmentType(EWeaponAttachmentTypes newType)
{
    _attachmentType = newType;
}