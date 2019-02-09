// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DroidAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UDroidAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	struct FDroidState DroidState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator CurrentFlinchRotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class ADroid *DroidRef;
};
