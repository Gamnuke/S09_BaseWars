// Fill out your copyright notice in the Description page of Project Settings.


#include "WheelAnimInstance.h"
#include "VehicleWheel.h"

void UWheelAnimInstance::NativeUpdateAnimation(float DeltaTime) {
	Super::NativeUpdateAnimation(DeltaTime);

	if (AssignedWheel != nullptr) {
		WheelLocation = AssignedWheel->Location;
		SteerAngle = AssignedWheel->GetSteerAngle();
		Speed = AssignedWheel->GetRotationAngle();
	}
}