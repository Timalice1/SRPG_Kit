#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FireWeaponInterface.h"
#include "Components/TimelineComponent.h"
#include "FireWeaponAnimInstance.generated.h"


UCLASS()
class SRPG_KIT_API UFireWeaponAnimInstance : public UAnimInstance, public IFireWeaponInterface
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Slider")
	FVector SlideMoveDirection;
	UPROPERTY(EditDefaultsOnly, Category = "Slider")
	float SlideAmount;
	UPROPERTY(EditDefaultsOnly, Category = "Slider")
	float SlideInterpSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Slider")
	float SlideBackInterpSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Slider")
	FVector SliderTranslation_Current;

	FTimeline SlideTimeline;

protected:

	UFUNCTION()
	void NativeBeginPlay() override;

	UFUNCTION()
	void Slide_Implementation(const bool CanSlideBack) ;
};
