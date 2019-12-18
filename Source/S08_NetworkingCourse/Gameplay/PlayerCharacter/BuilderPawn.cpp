// Fill out your copyright notice in the Description page of Project Settings.


#include "BuilderPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "GameMechanics/GamePlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
ABuilderPawn::ABuilderPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);

	PartImageHolder = CreateDefaultSubobject<USceneComponent>(FName("PartImageHolder"));
	PartImageHolder->SetupAttachment(GetRootComponent());

	StaticPartImage = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticPartImage"));
	StaticPartImage->SetupAttachment(PartImageHolder);

	SkeletalPartImage = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeletalPartImage"));
	SkeletalPartImage->SetupAttachment(PartImageHolder);
}

// Called when the game starts or when spawned
void ABuilderPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABuilderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABuilderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABuilderPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABuilderPawn::MoveRight);

	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &ABuilderPawn::PlaceItem);


	PlayerInputComponent->BindAxis("LookUp", this, &ABuilderPawn::MouseY);
	PlayerInputComponent->BindAxis("Turn", this, &ABuilderPawn::MouseX);

	PlayerInputComponent->BindAxis("Zoom", this, &ABuilderPawn::Zoom);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABuilderPawn::StartGrab);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABuilderPawn::StopGrab);

	PlayerInputComponent->BindAction("TurnPartPitch", IE_Pressed, this, &ABuilderPawn::RotatePartPitch);
	PlayerInputComponent->BindAction("TurnPartYaw", IE_Pressed, this, &ABuilderPawn::RotatePartYaw);
	PlayerInputComponent->BindAction("TurnPartRoll", IE_Pressed, this, &ABuilderPawn::RotatePartRoll);

	PlayerInputComponent->BindAction("PlaceTool", IE_Pressed, this, &ABuilderPawn::PlaceTool);
	PlayerInputComponent->BindAction("DeleteTool", IE_Pressed, this, &ABuilderPawn::DeleteTool);
	PlayerInputComponent->BindAction("ConfigureTool", IE_Pressed, this, &ABuilderPawn::ConfigureTool);
	PlayerInputComponent->BindAction("PaintTool", IE_Pressed, this, &ABuilderPawn::PaintTool);

}
void ABuilderPawn::PlaceTool() { SetTool(ECurrentTool::PlaceTool); }
void ABuilderPawn::DeleteTool() { SetTool(ECurrentTool::DeleteTool); }
void ABuilderPawn::ConfigureTool() { SetTool(ECurrentTool::ConfigureTool); }
void ABuilderPawn::PaintTool() { SetTool(ECurrentTool::PaintTool); }


void ABuilderPawn::RotatePartPitch() {
	GetPlayerController();
	if (PC == nullptr || PC->Menu == nullptr) { return; }
	PC->Menu->RotateItem(FRotator(90, 0, 0));

}
void ABuilderPawn::RotatePartYaw() {
	GetPlayerController();
	if (PC == nullptr || PC->Menu == nullptr) { return; }
	PC->Menu->RotateItem(FRotator(0, 90, 0));

}
void ABuilderPawn::RotatePartRoll() {
	GetPlayerController();
	if (PC == nullptr || PC->Menu == nullptr) { return; }
	PC->Menu->RotateItem(FRotator(0,0,90));

}

void ABuilderPawn::PlaceItem() {
	GetPlayerController();
	if (PC == nullptr || PC->Menu == nullptr) { return; }
	PC->Menu->OnLeftMouseClick();

}

void ABuilderPawn::GetPlayerController()
{
	if (PC == nullptr) {
		PC = Cast<AGamePlayerController>(GetNetOwningPlayer()->PlayerController);
	}
}

void ABuilderPawn::MoveForward(float Value) {
	FRotator NewRotator = SpringArm->GetComponentRotation();
	NewRotator.Pitch = FMath::Clamp(NewRotator.Pitch + 1 * Value, -89.0f,0.0f);
	SpringArm->SetWorldRotation(NewRotator);

}

void ABuilderPawn::MoveRight(float Value) {
	FRotator NewRotator = SpringArm->GetComponentRotation();
	NewRotator.Yaw = NewRotator.Yaw + 1 * Value;
	SpringArm->SetWorldRotation(NewRotator);
}

void ABuilderPawn::StartGrab() { 
	bGrabbing = true; 
	APlayerController *C = Cast<APlayerController>(Controller);
	C->bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(true);
	C->SetInputMode(InputMode);
}
void ABuilderPawn::StopGrab() { 
	bGrabbing = false;
	APlayerController *C = Cast<APlayerController>(Controller);
	C->bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	C->SetInputMode(InputMode);
}

void ABuilderPawn::MouseX(float Value) { if (bGrabbing) { MoveRight(Value); } }
void ABuilderPawn::MouseY(float Value) { if (bGrabbing) { MoveForward(Value*-1); } }

void ABuilderPawn::Zoom(float Value) {
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + 100 * Value, 10.0f, 4000.0f);
}

void ABuilderPawn::SetTool(ECurrentTool Tool)
{
	CurrentTool = Tool;
	if (MenuWidgetRef != nullptr) {
		MenuWidgetRef->CurrentTool = Tool;
	}
}

void ABuilderPawn::PostPlaceItem_Implementation(FVector Part)
{
}

