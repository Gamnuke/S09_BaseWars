// Fill out your copyright notice in the Description page of Project Settings.


#include "Wheel.h"

void UWheel::OnSelected()
{
	Super::OnSelected();
	AllowVariableEdit(PartSettings.ModifiableVariables.Acceleration);
	AllowVariableEdit(PartSettings.ModifiableVariables.Speed);
	AllowVariableEdit(PartSettings.ModifiableVariables.SteeringAngle);
	//e
}

