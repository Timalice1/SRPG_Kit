#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

UCLASS()
class INVENTORYANDWEAPONSYSTEM_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABaseItem();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* Mesh;

};
