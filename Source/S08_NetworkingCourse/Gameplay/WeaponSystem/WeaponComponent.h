// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "WeaponComponent.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
 struct FWeaponSettings {
 GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Functionality)
		float RoundsPerSecond = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Functionality)
		bool SemiAuto = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Functionality)
		bool UseShotgunAnimation = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Functionality)
		int ProjectilesPerShot = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Functionality)
		class UClass *ProjectileClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReticleEffects)
		float HipIdleCrosshairThreshold = 30;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReticleEffects)
		float AimIdleCrosshairThreshold = 15;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReticleEffects)
		float WalkCrosshairIncrement = 20;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReticleEffects)
		float FireCrosshairIncrement = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ReticleEffects)
		float CrosshairAimSpeed = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraEffects)
		float CameraFOVIncrement = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraEffects)
		float CameraFOVResetTime = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraEffects)
		float CameraZoomFOV = 60;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraEffects)
		float CameraPitchIncrement = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraEffects)
		float CameraSwayMaxDistance = 50;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraEffects)
		float CameraSwayMaxAngle = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraEffects)
		float CameraAngleSwayInterpSpeed = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraEffects)
		float CameraDistanceSwayInterpSpeed = 6;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effects)
		class UParticleSystem *MuzzleFlashClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effects)
		float MuzzleAngle = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effects)
		float MuzzleSpinRate = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Ammo)
		int MagazineCapacity = 30;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Ammo)
		int MaxAmmo = 240;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Ammo)
		float ReloadTime = 1;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Sound)
		class USoundBase *FireSound;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
		float BasePitch;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
		class USoundBase *WeaponInitiateSound;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
		class USoundBase *WeaponZoomInSound;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
		class USoundBase *WeaponZoomOutSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Melee)
		bool IsMeleeWeapon = false;
};

USTRUCT(BlueprintType)
struct FWeaponState {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Ammo)
		int CurrentMagazineAmmo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Ammo)
		int CurrentMaxAmmo;
};

USTRUCT(BlueprintType)
struct FProjectileSettings {
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool InstantImpact = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InstantImpactProjectileSettings)
		float SpeedMultiplier = 5; //Set this to 1 for a linear speed acceleration
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InstantImpactProjectileSettings)
		float InitialSpeed; // The initial speed of the projectile

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Damage = 23;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool ExplosionRadius = 0; // Set this to 0 for no explosion

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TrailThickness = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ImpactForce = 20000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ImpactSizeScale = 1;


};

UCLASS(Blueprintable)
class S08_NETWORKINGCOURSE_API UWeaponComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:
	UWeaponComponent(const FObjectInitializer & ObjectInitializer);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction);
	virtual void OnRegister();

	class AMainCharacter *Character;
	void SetCrosshairSize(float ThresholdToSet);

	

	void AimGun(bool Smooth);

	float CurrentDeltaTime;
public:
	void FireWeapon();

	float CurrentThreshold;
	float TargetThreshold;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWeaponSettings WeaponSettings;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FProjectileSettings ProjectileSettings;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UStaticMesh* Mesh;

	float CurrentMuzzleAngle;

	FVector AimLocation;
	FVector AimDirection;

	class UParticleSystemComponent *MuzzleFlashSystem;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool CanShoot;

	int CurrentMagazineAmmo;
	int CurrentMaxAmmo;

	void ReloadWeapon(bool RefilAmmo);
	void SetWeaponState(FWeaponState State);

	TSubclassOf<class UWeaponComponent> WeaponSubclass;

	FWeaponState WeaponState;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool CanCauseMeleeDamage = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	float TimeUntilCanMeleeDamage;

	TArray<class ADroid*> ImpactedDroids;
};
