// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/WeaponSystem/WeaponComponent.h"
#include "Weapon.generated.h"

/**
 * 
 */

USTRUCT()
struct FMovementState {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FVector Location;
	UPROPERTY()
		FVector Velocity;
	UPROPERTY()
		FRotator Rotation;
};

UCLASS()
class S08_NETWORKINGCOURSE_API AWeapon : public AActor
{
	GENERATED_BODY()

AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UStaticMeshComponent *Mesh;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class UWeaponComponent> AssignedWeapon;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
		FWeaponState AssignedWeaponState;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool SelfInitialize = true;
};
