#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttachmentsButton_Base.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedEvent);

UCLASS(Abstract)
class SRPG_KIT_API UAttachmentsButton_Base : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;

    UFUNCTION()
    virtual void OnClick();

    UPROPERTY(EditAnywhere, Category = ButtonConfig)
    FText ButtonLabel_Text;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnClickedEvent OnClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton *button;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *buttonLabel;
};