#include "BaseItem.h"
#include "Components/SphereComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = SphereCollision;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	Mesh->AttachToComponent(SphereCollision, FAttachmentTransformRules::KeepRelativeTransform);

}
