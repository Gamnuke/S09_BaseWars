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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RoundsPerSecond = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool SemiAuto = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float HipIdleCrosshairThreshold = 100;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AimIdleCrosshairThreshold = 50;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float WalkCrosshairIncrement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FireCrosshairIncrement = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float CameraFOVIncrement = -5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float CameraFOVResetTime = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float CameraPitchIncrement = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UClass *ProjectileClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UParticleSystem *MuzzleFlashClass;


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

	float CurrentThreshold;
	float TargetThreshold;
public:
	UFUNCTION()
		void FireWeapon();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWeaponSettings WeaponSettings;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UStaticMesh* Mesh;

	TOptional<FVector> AimLocation;
	TOptional<FVector> AimDirection;

	class UParticleSystemComponent *MuzzleFlashSystem;
};
