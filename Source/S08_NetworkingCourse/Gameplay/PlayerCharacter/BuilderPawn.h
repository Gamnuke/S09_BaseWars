// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BuilderPawn.generated.h"

UENUM(BlueprintType)
enum ECurrentTool {

	PlaceTool, DeleteTool, ConfigureTool, PaintTool
};

UCLASS()
class S08_NETWORKINGCOURSE_API ABuilderPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABuilderPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void PlaceTool();

	UFUNCTION()
		void DeleteTool();

	UFUNCTION()
		void ConfigureTool();

	UFUNCTION()
		void PaintTool();

	UFUNCTION()
		void RotatePartPitch();

	UFUNCTION()
		void RotatePartYaw();

	UFUNCTION()
		void RotatePartRoll();

	UFUNCTION()
		void PlaceItem();
	void GetPlayerController();
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);
	UFUNCTION()
	void StartGrab();
	UFUNCTION()
	void StopGrab();

	UFUNCTION()
		void MouseX(float Value);
	UFUNCTION()
		void MouseY(float Value);

	UFUNCTION()
		void Zoom(float Value);
	class AGamePlayerController *PC;
	bool bGrabbing = false;
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent *PartImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool CanPlace;

	UFUNCTION(BlueprintNativeEvent)
		void PostPlaceItem();

	void SetTool(ECurrentTool Tool);

	ECurrentTool CurrentTool;

	class UMenu *MenuWidgetRef;
};
