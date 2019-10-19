// Fill out your copyright notice in the Description page of Project Settings.


#include "WheeledVehicleTest.h"
#include "WheeledVehicleMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SimpleWheeledVehicleMovementComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

AWheeledVehicleTest::AWheeledVehicleTest(const FObjectInitializer& ObjectInitializer) {
	NumWheels = FMath::RoundToInt(NumWheels / 2) * 2;
	MovementComp = CreateDefaultSubobject<USimpleWheeledVehicleMovementComponent>(FName("Comp"));
	MovementComp->UpdatedComponent = GetMesh();

	Box = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	Box->SetupAttachment(GetMesh());

	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComp->SetupAttachment(GetMesh());

	LineComponent = CreateDefaultSubobject<ULineBatchComponent>(FName("LineDrawComponent"));
	LineComponent->SetupAttachment(BoxComp);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(BoxComp);

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AWheeledVehicleTest::GenerateCar() {
	///GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(1, 10, FColor::Red, FString("Working"));
	MovementComp->UpdatedComponent = GetMesh();
	TArray<FWheelSetup> Setups;
	// Left Wheels
	for (int32 i = 0; i < NumWheels/2; i++)
	{
		FWheelSetup NewSetupLeft = FWheelSetup();
		NewSetupLeft.BoneName = *(FString("Wheel_") + FString::FromInt(i + NumWheels / 2));
		NewSetupLeft.AdditionalOffset = FVector(100 * i - ((((NumWheels/2)-1)*100) / 2), -100, 0);
		Setups.Add(NewSetupLeft);

		FWheelSetup NewSetupRight = FWheelSetup();
		NewSetupRight.BoneName = *(FString("Wheel_") + FString::FromInt(i));
		NewSetupRight.AdditionalOffset = FVector(100 * i - ((((NumWheels/2)-1) * 100) / 2), 100, 0);
		Setups.Add(NewSetupRight);
	}
	MovementComp->WheelSetups = Setups;
	MovementComp->CreateVehicle();
	MovementComp->RecreatePhysicsState();
}