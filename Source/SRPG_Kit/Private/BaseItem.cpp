#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	InteractionCollision->SetCollisionProfileName("OverlapAllDynamic");
	RootComponent = InteractionCollision;

	RootPoint = CreateDefaultSubobject<USceneComponent>("MeshOffset");
	RootPoint->AttachToComponent(InteractionCollision, FAttachmentTransformRules::KeepRelativeTransform);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	Mesh->AttachToComponent(RootPoint, FAttachmentTransformRules::KeepRelativeTransform);
	Mesh->SetCollisionProfileName("BlockAllDynamic");
}

void ABaseItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (Mesh->IsAnySimulatingPhysics() && Mesh->GetComponentVelocity().Length() != 0)
		SetActorLocation(Mesh->GetComponentLocation());
}