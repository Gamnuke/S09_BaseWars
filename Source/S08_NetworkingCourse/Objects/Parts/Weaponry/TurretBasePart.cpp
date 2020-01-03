// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretBasePart.h"

void UTurretBasePart::OnSelected()
{
	Super::OnSelected();

	AllowVariableEdit(PartSettings.ModifiableVariables.RotationSpeed);

}