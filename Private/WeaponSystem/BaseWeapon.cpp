#include "WeaponSystem/BaseWeapon.h"

ABaseWeapon::ABaseWeapon()
{
	Range = 10000.f;
}

void ABaseWeapon::GetHandsIKTransform(const USkeletalMeshComponent* CharacterMesh, 
	FName RightHandRelativeBone, 
	FName LeftHandRelativeBone,
	FTransform& LeftHand,
	FTransform& RightHand) const {

	auto leftHandSocket = Mesh->GetSocketTransform("LeftHandSocket");
	auto rightHandSocket = Mesh->GetSocketTransform("RightHandSocket");

	/*Transform right hand to bone space*/
	FVector rightHand_location;
	FRotator rightHand_rotation;
	CharacterMesh->TransformToBoneSpace(RightHandRelativeBone,
		rightHandSocket.GetLocation(),
		rightHandSocket.Rotator(),
		rightHand_location,
		rightHand_rotation);

	/*Transform left hand to bone space*/
	FVector leftHand_location;
	FRotator leftHand_rotation;
	CharacterMesh->TransformToBoneSpace(LeftHandRelativeBone,
		leftHandSocket.GetLocation(),
		leftHandSocket.Rotator(),
		leftHand_location,
		leftHand_rotation);

	RightHand = FTransform(rightHand_rotation, rightHand_location);
	LeftHand = FTransform(leftHand_rotation, leftHand_location);
}