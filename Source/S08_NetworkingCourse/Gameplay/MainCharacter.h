// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MenuSystem/InGameUI/ChatSystem/ChatDisplayWidget.h"
#include "UnrealNetwork.h"
#include "Styling/SlateColor.h"
#include "Misc/Optional.h"

#include "MainCharacter.generated.h"

USTRUCT()
struct FState{
	GENERATED_USTRUCT_BODY();

	UPROPERTY()
		bool NewSprinting;

	UPROPERTY()
		bool NewAiming;

	UPROPERTY()
		bool NewFalling;

	UPROPERTY()
		bool NewWeaponEquipped;

	UPROPERTY()
	bool NewFiringWeapon;

	UPROPERTY()
		float NewAimPitch;

	UPROPERTY()
		FVector NewVelocity;
	UPROPERTY()
		FVector NewRotVelocity;
	
	UPROPERTY()
		FRotator NewMeshRotation;
	UPROPERTY()
		FRotator MeshRelativeRotation;

	UPROPERTY()
		FRotator NewControlRotation;
	
};

UCLASS()
class S08_NETWORKINGCOURSE_API AMainCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent *WidgetComponent;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Widgets)
		class UChatDisplayWidget *ChatDisplayWidget;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Widgets)
		class UInGameHUD *InGameHUD;

	UPROPERTY(ReplicatedUsing=OnRep_ColorAssigned, VisibleAnywhere, BlueprintReadWrite, Category = Other)
		FLinearColor AssignedColor;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
		FState ServerState;

	UFUNCTION(Server, reliable, WithValidation)
	void SetServerState(FState NewState);

	void SetState(FState NewState);
public:
	bool Aiming;
	bool Sprinting;
	bool Falling;
	bool WeaponEquipped;
	bool FiringWeapon;

	float AimPitch;

	FVector Velocity;
	FVector RotVelocity;

	FRotator TargetRotation;
	FRotator ControlRotation;

	FRotator MeshRotation;
	FRotator MeshRelativeRotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float WalkSpeed = 400;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float SprintSpeed = 800;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float AimWalkSpeed = 300;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float CrouchSpeed = 300;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;



public:
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION()
	void OpenPauseMenu();

	UFUNCTION(Server, WithValidation, unreliable, BlueprintCallable)
	void Server_CreateChatDisplay(const FText &PlayerName, const FText &Message, FLinearColor NewAssignedColor);

	UFUNCTION(NetMulticast, unreliable, BlueprintCallable)
	void Multicast_CreateChatDisplay(const FText &PlayerName, const FText &Message, FLinearColor NewAssignedColor);

	UFUNCTION()
		virtual void OnRep_ColorAssigned();

	UFUNCTION()
		void OnRep_ServerState();
private:
	TSubclassOf<class UUserWidget> ChatDisplayWidgetClass;
	TSubclassOf<class UUserWidget> ChatDisplayTabClass;
	TSubclassOf<class UUserWidget> ChatTabClass;
	TSubclassOf<class UUserWidget> InGameHUDClass;
	float NextDisplayUpdate;
public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class UWeaponComponent> WeaponComponentClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UWeaponComponent* Weapon;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartSprinting();
	void StopSprinting();
	void StartAiming();
	void StopAiming();
	void EquipWeapon();
	void StartFireWeapon();
	void StopFireWeapon();

	void AnimNotify_Fire();
};
