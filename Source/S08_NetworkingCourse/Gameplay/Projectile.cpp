// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

#include "Gameplay/MainCharacter.h"
#include "Gameplay/MainPlayerState.h"
#include "Gameplay/Droid.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "MenuSystem/InGameUI/InGameHUD.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileTrail = CreateDefaultSubobject<UParticleSystemComponent>(FName("Trail"));
	ProjectileTrail->SetupAttachment(RootComponent);

	ProjectileImpact = CreateDefaultSubobject<UParticleSystemComponent>(FName("Impact"));
	ProjectileImpact->SetupAttachment(ProjectileTrail);

	DroidImpact = CreateDefaultSubobject<UParticleSystemComponent>(FName("DroidImpact"));
	DroidImpact->SetupAttachment(ProjectileTrail);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
	Speed = ProjectileSettings.InitialSpeed;
	CallDestructionDelay();

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
			ProjectileImpact->SetWorldScale3D(FVector::OneVector * ProjectileSettings.ImpactSizeScale);
			ProjectileImpact->Activate();

			if (ProjectileSettings.InstantImpact) {
				AActor *NewTrail = GetWorld()->SpawnActor<AActor>(Trail.Get(), SweepResult.TraceStart, GetActorRotation());
				NewTrail->SetActorScale3D(FVector(ProjectileSettings.TrailThickness, ProjectileSettings.TrailThickness, ((GetActorLocation() - StartLocation).Size()) / 100));
				NewTrail->SetActorLocation(StartLocation + (GetActorLocation() - StartLocation) / 2);
			}

			if (SweepResult.GetActor() == nullptr) { return; }
			ADroid *Droid = Cast<ADroid>(SweepResult.GetActor());
			if (Droid == nullptr) {
				return;
			}

			Droid->LeadFlinchRotation = Droid->LeadFlinchRotation + FRotator(FMath::RandRange(-45,45), FMath::RandRange(-45, 45), FMath::RandRange(-45, 0));

			if (SweepResult.BoneName == FName("head")) {
				if (HasAuthority() && OwnedByServer) {
					Droid->DamageDroid(ProjectileSettings.Damage * 2, SweepResult);
				}
					if (HudRef != nullptr) {
						HudRef->Hitmarker->SetColorAndOpacity(FLinearColor(1, 0, 0, 1));
						HudRef->Hitmarker->SetRenderOpacity(20);
					}
			}
			else {
				if (HasAuthority() && OwnedByServer) {
					Droid->DamageDroid(ProjectileSettings.Damage, SweepResult);
				}
					if (HudRef != nullptr) {
						HudRef->Hitmarker->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
						HudRef->Hitmarker->SetRenderOpacity(20);
					}
			}

			Droid->GetMesh()->AddImpulseAtLocation((SweepResult.ImpactPoint - SweepResult.TraceStart).GetSafeNormal() * 1000, SweepResult.ImpactPoint);
			DroidImpact->SetWorldRotation(SweepResult.ImpactNormal.Rotation());
			DroidImpact->SetWorldLocation(SweepResult.ImpactPoint);
			DroidImpact->Activate();
			ProjectileImpact->Deactivate();
		}
	}

	if (Speed > 60000) {
		Destroy();
	}
}


void AProjectile::CallDestructionDelay_Implementation(){}

