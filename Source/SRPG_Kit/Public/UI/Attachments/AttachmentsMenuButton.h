#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttachmentsMenuButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedSignature, class UAttachmentsMenuButton *, clickedButton);

UCLASS()
class SRPG_KIT_API UAttachmentsMenuButton : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;

public:
    UPROPERTY(EditAnywhere, Category = ButtonConfig)
    FText buttonLabelText = FText::FromString("Button");

    UPROPERTY(BlueprintAssignable)
    FOnClickedSignature OnClick;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> button;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> buttonLabel;

    UFUNCTION()
    void OnButtonClicked();
};