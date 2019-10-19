// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SimulatedVehicle.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API ASimulatedVehicle : public APawn
{
	GENERATED_BODY()
		ASimulatedVehicle(const FObjectInitializer& ObjectInitializer);

	USkeletalMeshComponent * GetMesh();

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void MoveForward(float Input);

	UFUNCTION()
		void MoveRight(float Input);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UBoxComponent *BoxComp;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USpringArmComponent *SpringArm;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UCameraComponent *Camera;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USimpleWheeledVehicleMovementComponent *MovementComp;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USkeletalMeshComponent *SkeleMesh;

	class UBoxComponent* CockpitBox;

	virtual void Tick(float DeltaTime) override;
};
