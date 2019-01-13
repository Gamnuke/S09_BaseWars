// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileTrail = CreateDefaultSubobject<UParticleSystemComponent>(FName("Trail"));
	ProjectileTrail->SetupAttachment(RootComponent);

	ProjectileImpact = CreateDefaultSubobject<UParticleSystemComponent>(FName("Impact"));
	ProjectileImpact->SetupAttachment(ProjectileTrail);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
	Speed = ProjectileSettings.InitialSpeed;
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMove) {
		Speed = Speed * ProjectileSettings.SpeedMultiplier;
		FHitResult SweepResult;

		GetWorld()->LineTraceSingleByChannel(
			SweepResult,
			GetActorLocation(),
			GetActorLocation() + GetActorForwardVector() * (ProjectileSettings.InstantImpact? 100000 : Speed),
			ECollisionChannel::ECC_Visibility
		);

		if (!SweepResult.IsValidBlockingHit()) {
			SetActorLocation(SweepResult.TraceEnd - GetActorForwardVector());
			
		}
		else {
			CanMove = false;
			SetActorLocation(SweepResult.ImpactPoint);
			ProjectileTrail->Deactivate();
			ProjectileImpact->SetWorldRotation(SweepResult.ImpactNormal.Rotation());
			
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + SweepResult.ImpactNormal * 10, FColor::Red, false, 10);
			ProjectileImpact->Activate();

			if (ProjectileSettings.InstantImpact) {
				AActor *NewTrail = GetWorld()->SpawnActor<AActor>(Trail.Get(), SweepResult.TraceStart, GetActorRotation());
				NewTrail->SetActorScale3D(FVector(1, 1, ((GetActorLocation() - StartLocation).Size()) / 100));
				NewTrail->SetActorLocation(StartLocation + (GetActorLocation() - StartLocation) / 2);
			}

			CallDestructionDelay();
		}
	}

	if (Speed > 60000) {
		Destroy();
	}
}

void AProjectile::CallDestructionDelay_Implementation(){}

