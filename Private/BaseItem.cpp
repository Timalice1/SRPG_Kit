#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = Root;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	Mesh -> AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollision->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);

}
