#include "UI/Attachments/AttachmentsMenuButton.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UAttachmentsMenuButton::NativePreConstruct()
{
    Super::NativePreConstruct();
    buttonLabel->SetText(buttonLabelText);

    button->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
}

void UAttachmentsMenuButton::NativeConstruct()
{
    Super::NativeConstruct();
}

void UAttachmentsMenuButton::OnButtonClicked()
{
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("Some button clicked")));
    OnClick.Broadcast(this);
}