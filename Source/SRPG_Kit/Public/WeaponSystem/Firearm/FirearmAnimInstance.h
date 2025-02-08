#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FirearmAnimInstance.generated.h"

UCLASS()
class SRPG_KIT_API UFirearmAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void Shoot();

private:
	UPROPERTY()
	TObjectPtr<class AActor> _ownerActor = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireWeapon|Slider")
	FVector TargetSlideTranslation = FVector();

	UPROPERTY(EditAnywhere, Category = "FireWeapon|Slider")
	float SlideTranslationInterpSpeed = 10.f;

	UPROPERTY(BlueprintReadOnly, Category = "FireWeapon|Slider")
	float SlideTranslationAlpha = 0.f;
};
