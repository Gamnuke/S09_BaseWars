// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MenuSystem/InGameUI/ChatSystem/ChatDisplayWidget.h"
#include "UnrealNetwork.h"
#include "Styling/SlateColor.h"
#include "Misc/Optional.h"
#include "Gameplay/Projectile.h"


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
		bool Taunting;

	UPROPERTY()
		bool NewFiringWeapon;

	UPROPERTY()
		bool ReloadingWeapon;

	UPROPERTY()
		bool FireAnimPlaying;

	UPROPERTY()
		float NewAimPitch;

	UPROPERTY()
		int CurrentWeaponIndex;

	UPROPERTY()
		int PreviousWeaponIndex;

	UPROPERTY()
		bool SwitchingWeapon;

	UPROPERTY()
		FVector NewVelocity;
	UPROPERTY()
		FVector NewRotVelocity;
	UPROPERTY()
		FVector NewProjectedLocation;

	UPROPERTY()
		FRotator NewMeshRotation;
	UPROPERTY()
		FRotator MeshRelativeRotation;
	UPROPERTY()
		FVector NewProjectedDirection;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent *PrimaryWeaponModel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent *SecondaryWeaponModel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		TArray<class UWeaponComponent*> WeaponModels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> SocketNames;

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

	bool WeaponEquipped;
	bool FiringWeapon;
	bool SwitchingWeapon;
	bool ReloadingWeapon;
	bool FireAnimPlaying;

	bool Falling;
	bool Sprinting;
	bool Taunting;

	int CurrentWeaponIndex = 0;
	int PreviousWeaponIndex = 0;

	class UWeaponComponent* CurrentWeapon = nullptr;

	bool Idle;

	float AimPitch;
	int CurrentHealth = 100;

	FVector Velocity;
	FVector RotVelocity;
	FVector ProjectedLocation;
	FVector ProjectedDirection;

	FRotator TargetRotation;
	FRotator ControlRotation;

	FRotator MeshRotation;
	FRotator MeshRelativeRotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float WalkSpeed = 300;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float SprintSpeed = 700;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float AimWalkSpeed = 300;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float CrouchSpeed = 300;

	FVector WeaponGripLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float DefaultCameraFOV = 80;

	float CameraTargetFOV;
	float TargetCameraPitch;

	FVector TargetCameraRelativeLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	float NextCanHealthRegenTime;
	float NextHealthRegenTime;

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
	TArray<TSubclassOf<class UWeaponComponent>> Weapons;


	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Jump();
	void StartSprinting();
	void StopSprinting();
	void StartAiming();
	void StopAiming();
	void EquipWeapon();
	void StartFireWeapon();
	void StopFireWeapon();
	void ScrollUp();
	void ScrollDown();
	void StartTaunting();
	void StartReloadingWeapon();

	UFUNCTION(Server, WithValidation, Reliable)
	void DropWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDropWeapon();

	UFUNCTION(Server, WithValidation, Reliable)
	void ReplaceWeapon();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastReplaceWeapon();

	void ReloadWeapon();

	bool Dead;

	void DamagePlayer(int Amount);

	void AnimNotify_Fire();

	void StartSwitchingWeapon();
	void PlacedWeapon();

	void SwitchWeapon();
	void FinishedSwitchingWeapon();

	void SetMeshMaterial(UStaticMeshComponent *Mesh, UWeaponComponent *MeshToSetMaterial);

	void SpawnProjectile(FTransform Transform, TSubclassOf<class AProjectile> Projectile, struct FProjectileSettings Settings);

	void UpdateAmmoDisplay(int MagazineAmmo, int MaxAmmo);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnProjectile(FTransform Transform, TSubclassOf<class AProjectile> Projectile, struct FProjectileSettings Settings);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSpawnProjectile(FTransform Transform, TSubclassOf<class AProjectile> Projectile, struct FProjectileSettings Settings);

	UFUNCTION(NetMulticast, Reliable)
		void SetHealth(float Health, bool Heal);

	class AWeapon *HoveredWeapon;
};
