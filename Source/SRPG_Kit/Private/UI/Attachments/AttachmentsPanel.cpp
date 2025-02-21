#include "UI/Attachments/AttachmentsPanel.h"
#include "UI/Attachments/AttachmentsMenuButton.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UAttachmentsPanel::NativeConstruct()
{
    Super::NativeConstruct();

    panelLabel->SetText(panelLabel_Text);
}