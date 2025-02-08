#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FirearmAnimInstance.generated.h"

UCLASS()
class SRPG_KIT_API UFirearmAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void Shoot();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireWeapon|Slider")
	FVector TargetSlideTranslation = FVector();
	UPROPERTY(EditAnywhere, Category = "FireWeapon|Slider")
	float SlideTranslationInterpSpeed = 10.f;

private:
	float SlideTranslationAlpha = 0.f;
};
