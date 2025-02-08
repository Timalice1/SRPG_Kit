#include "WeaponSystem/Firearm/FirearmAnimInstance.h"
#include "WeaponSystem/Firearm/FireWeaponInterface.h"

void UFirearmAnimInstance::NativeBeginPlay()
{
    if (GetOwningActor()->GetClass()->ImplementsInterface(UFireWeaponInterface::StaticClass()))
        _ownerActor = GetOwningActor();
}

void UFirearmAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    if (_ownerActor && IFireWeaponInterface::Execute_GetCurrentAmmo(_ownerActor) != 0)
        SlideTranslationAlpha = FMath::FInterpTo(SlideTranslationAlpha, 0.f, DeltaSeconds, SlideTranslationInterpSpeed);
}

void UFirearmAnimInstance::Shoot()
{
    SlideTranslationAlpha = 1.f;
}
