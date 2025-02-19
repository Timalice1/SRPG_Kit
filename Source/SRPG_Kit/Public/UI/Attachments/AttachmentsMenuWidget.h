#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttachmentsMenuWidget.generated.h"

UCLASS()
class SRPG_KIT_API UAttachmnetsMenuWidget : public UUserWidget
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<class AFireWeapon> weaponRef;

public:
    virtual void NativeConstruct() override {};

    UFUNCTION(BlueprintCallable, Category = AttachmentsMenuWidget)
    void SetWeaponRef(class AFireWeapon *weapon) { weaponRef = weapon; };

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UVerticalBox> slotsList;
};