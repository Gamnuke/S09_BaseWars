// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

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
		float Damage = 23;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool ExplosionRadius = 0; // Set this to 0 for no explosion


};

UCLASS()
class S08_NETWORKINGCOURSE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	bool CanMove = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FProjectileSettings ProjectileSettings;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UParticleSystemComponent *ProjectileTrail;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UParticleSystemComponent *ProjectileImpact;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AActor> Trail;

	UFUNCTION(BlueprintNativeEvent)
		void CallDestructionDelay();


	virtual void CallDestructionDelay_Implementation();


	float Speed = 10;

	FVector StartLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
