// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulatedVehicle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SimpleWheeledVehicleMovementComponent.h"


ASimulatedVehicle::ASimulatedVehicle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*PartImage = CreateDefaultSubobject<UStaticMeshComponent>(FName("PartImage"));
	PartImage->SetupAttachment(GetRootComponent());*/

	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	SetRootComponent(BoxComp);

	SkeleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeleMesh"));
	SkeleMesh->SetupAttachment(BoxComp);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(BoxComp);
	SpringArm->TargetArmLength = 2000;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);

	MovementComp = CreateDefaultSubobject<USimpleWheeledVehicleMovementComponent>(FName("MainVehicleMovementComponent"));
	MovementComp->UpdatedComponent = SkeleMesh;

	SkeleMesh->SetSimulatePhysics(true);
	MovementComp->UpdatedPrimitive = BoxComp;
}
USkeletalMeshComponent *ASimulatedVehicle::GetMesh() {
	return SkeleMesh;
}
void ASimulatedVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASimulatedVehicle::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASimulatedVehicle::MoveRight);
}
void ASimulatedVehicle::MoveForward(float Input) {
	int32 i = 0;
	for (FWheelSetup Setup : MovementComp->WheelSetups) {
		MovementComp->SetDriveTorque(Input * 10000, i);
		i++;
	}

}
void ASimulatedVehicle::MoveRight(float Input) {
	int32 i = 0;
	for (FWheelSetup Setup : MovementComp->WheelSetups) {
		if (i< MovementComp->WheelSetups.Num()/2) {
			MovementComp->SetSteerAngle(Input*30, i);
		}
		i++;
	}
}

void ASimulatedVehicle::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (CockpitBox != nullptr) {
		SpringArm->SetWorldLocation(CockpitBox->GetComponentLocation());
		SpringArm->SetupAttachment(CockpitBox);
	}
}