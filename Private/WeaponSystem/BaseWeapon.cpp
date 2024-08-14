#include "WeaponSystem/BaseWeapon.h"

ABaseWeapon::ABaseWeapon()
{
	Range = 10000.f;
	BaseDamage = 1.f;
}

void ABaseWeapon::GetHandsIK_Transform(const USkeletalMeshComponent* CharacterMesh,
	FName RightHandSocketName,
	FName LeftHandSocketName,
	FTransform& RightHandTransform,
	FTransform& LeftHandTransform) const {

	FTransform rightHandSocket = Mesh->GetSocketTransform(RightHandSocketName);

	/*Transform right hand to bone space*/
	FVector rightHand_location;
	FRotator rightHand_rotation;
	CharacterMesh->TransformToBoneSpace("ik_hand_gun",
		rightHandSocket.GetLocation(),
		rightHandSocket.Rotator(),
		rightHand_location,
		rightHand_rotation);

	RightHandTransform = FTransform(rightHand_rotation, rightHand_location);
	//if(!anyWeaponAttachments(Grip, Grenade thrower, etc)):
	LeftHandTransform = Mesh->GetSocketTransform(LeftHandSocketName, RTS_Component);
	//else: get transform from attachment mesh
}