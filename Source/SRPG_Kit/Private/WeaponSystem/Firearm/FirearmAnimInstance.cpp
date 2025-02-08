#include "WeaponSystem/Firearm/FirearmAnimInstance.h"

void UFirearmAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    SlideTranslationAlpha = FMath::FInterpTo(SlideTranslationAlpha, 0.f, DeltaSeconds, SlideTranslationInterpSpeed);
}

void UFirearmAnimInstance::Shoot()
{
    SlideTranslationAlpha = 1.f;
}
