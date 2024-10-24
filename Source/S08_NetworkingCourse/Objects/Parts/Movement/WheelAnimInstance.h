// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WheelAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UWheelAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		class UVehicleWheel *AssignedWheel;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FVector WheelLocation;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float SteerAngle;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float Speed;
};
