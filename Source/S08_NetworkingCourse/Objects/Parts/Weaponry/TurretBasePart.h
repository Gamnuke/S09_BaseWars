// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Parts/Part.h"
#include "TurretBasePart.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UTurretBasePart : public UPart
{
	GENERATED_BODY()

public:
	virtual void OnSelected() override;
};
