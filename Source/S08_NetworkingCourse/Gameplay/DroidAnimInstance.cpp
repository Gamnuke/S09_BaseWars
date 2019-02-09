// Fill out your copyright notice in the Description page of Project Settings.

#include "DroidAnimInstance.h"
#include "Gameplay/Droid.h"

void UDroidAnimInstance::NativeUpdateAnimation(float DeltaTimeX) {
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (DroidRef == nullptr) {
		APawn *Pawn = TryGetPawnOwner();
		if (Pawn == nullptr) { return; }
		DroidRef = Cast<ADroid>(Pawn);
	}
	if (DroidRef == nullptr) { return; }
	DroidState = DroidRef->DroidState;
	CurrentFlinchRotation = DroidRef->CurrentFlinchRotation;
}
