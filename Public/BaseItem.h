#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class USceneComponent;
class USkeletalMeshComponent;
class USphereComponent;

UCLASS()
class SRPG_KIT_API ABaseItem : public AActor
{
	GENERATED_BODY()

public:
	ABaseItem();

	void Tick(float DeltaSeconds) override;

protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh;
	
};
