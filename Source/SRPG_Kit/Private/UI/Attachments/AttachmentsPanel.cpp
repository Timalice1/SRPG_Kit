#include "UI/Attachments/AttachmentsPanel.h"
#include "Components/TextBlock.h"

void UAttachmentsPanel::NativePreConstruct()
{
    Super::NativePreConstruct();
    panelLabel->SetText(PanelLabel_Text);
}