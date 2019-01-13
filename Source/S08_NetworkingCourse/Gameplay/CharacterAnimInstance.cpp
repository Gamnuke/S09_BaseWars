// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAnimInstance.h"
#include "Gameplay/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTimeX) {
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (CharacterRef == nullptr) {
		APawn *Pawn = TryGetPawnOwner();
		if (Pawn == nullptr) { return; }
		CharacterRef = Cast<AMainCharacter>(Pawn);
	}
	if (CharacterRef == nullptr) { return; }
	
	Direction = CalculateDirection(CharacterRef->GetVelocity(), CharacterRef->GetMesh()->GetComponentRotation() + FRotator(0, 90, 0));
	FVector PlaneVel = CharacterRef->GetVelocity();
	PlaneVel.Z = 0;
	Speed = PlaneVel.SizeSquared();

	Aiming = CharacterRef->Aiming;
	Sprinting = CharacterRef->Sprinting;
	AimPitch = CharacterRef->AimPitch;
	Falling = CharacterRef->GetCharacterMovement()->IsFalling();
	WeaponEquipped = CharacterRef->WeaponEquipped;
	FiringWeapon = CharacterRef->FiringWeapon;
	GripLocation = CharacterRef->WeaponGripLocation;

	if (CharacterRef->CurrentWeapon != nullptr) {
		WeaponSettings = CharacterRef->CurrentWeapon->WeaponSettings;
	}
}

void UCharacterAnimInstance::AnimNotify_Fire()
{
	if (CharacterRef == nullptr) { return; }
	CharacterRef->AnimNotify_Fire();
}
