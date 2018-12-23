// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAnimInstance.h"
#include "Gameplay/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTimeX) {
	Super::NativeUpdateAnimation(DeltaTimeX);

	APawn *Pawn = TryGetPawnOwner();
	if (Pawn == nullptr) { return; }
	AMainCharacter *Character = Cast<AMainCharacter>(Pawn);
	if (Character == nullptr) { return; }
	
	Direction = CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());
	FVector PlaneVel = Character->GetVelocity();
	PlaneVel.Z = 0;
	Speed = PlaneVel.SizeSquared();

	Aiming = Character->Aiming;
	Sprinting = Character->Sprinting;
	AimPitch = Character->AimPitch;
	Falling = Character->GetCharacterMovement()->IsFalling();
}


