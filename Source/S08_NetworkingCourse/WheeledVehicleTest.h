// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "WheeledVehicleTest.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API AWheeledVehicleTest : public AWheeledVehicle
{
	GENERATED_BODY()
public:
	AWheeledVehicleTest(const FObjectInitializer& ObjectInitializer);
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 NumWheels;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class USimpleWheeledVehicleMovementComponent *MovementComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBoxComponent* Box;
	UFUNCTION(BlueprintCallable)
	void GenerateCar();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBoxComponent *BoxComp;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class USpringArmComponent *SpringArm;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UCameraComponent *Camera;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class ULineBatchComponent *LineComponent;
};
