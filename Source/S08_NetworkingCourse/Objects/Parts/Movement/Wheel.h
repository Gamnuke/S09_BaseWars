// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Parts/Part.h"
#include "Wheel.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UWheel : public UPart
{
	GENERATED_BODY()
public:
	virtual void OnSelected() override;
};
