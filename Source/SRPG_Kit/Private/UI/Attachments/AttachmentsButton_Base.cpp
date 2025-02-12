#include "UI/Attachments/AttachmentsButton_Base.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UAttachmentsButton_Base::NativePreConstruct()
{
    buttonLabel->SetText(ButtonLabel_Text);
}

void UAttachmentsButton_Base::NativeConstruct()
{
    Super::NativeConstruct();

    if(button)
        button->OnClicked.AddDynamic(this, &ThisClass::OnClick);

}

void UAttachmentsButton_Base::OnClick()
{
    GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString::Printf(TEXT("ButtonClicked")));
    OnClicked.Broadcast();
}
