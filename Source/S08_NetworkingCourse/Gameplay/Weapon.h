// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/WeaponComponent.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API AWeapon : public AActor
{
	GENERATED_BODY()

AWeapon();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UStaticMeshComponent *Mesh;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UWeaponComponent *AssignedWeaponComponent;
};
