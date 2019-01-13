// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Gameplay/MainCharacter.h"
#include "MenuSystem/InGameUI/InGameHUD.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Gameplay/Projectile.h"

#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

#include "Kismet/KismetMathLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"

UWeaponComponent::UWeaponComponent(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	if (Mesh != nullptr) {
		this->SetStaticMesh(Mesh);
	}
	PrimaryComponentTick.bCanEverTick = true;

	
}

void UWeaponComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Character == nullptr) { return; }

	TargetThreshold = Character->Aiming ? WeaponSettings.AimIdleCrosshairThreshold : WeaponSettings.HipIdleCrosshairThreshold;

	if (!Character->Idle) {
		TargetThreshold += Character->Aiming ? WeaponSettings.WalkCrosshairIncrement : WeaponSettings.WalkCrosshairIncrement * 2;
		if (Character->Sprinting) {
			TargetThreshold += 200;
		}
	}

	CurrentThreshold = FMath::FInterpTo(CurrentThreshold, TargetThreshold, DeltaTime, 5);

	FHitResult AimHit;
	GetWorld()->LineTraceSingleByChannel(
		AimHit,
		AimLocation.GetValue(),
		AimLocation.GetValue() + AimDirection.GetValue() * 100000,
		ECollisionChannel::ECC_Visibility
	);
	

	if (Character->WeaponEquipped && !Character->Sprinting) {
		if (AimHit.IsValidBlockingHit()) {
			SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), AimHit.ImpactPoint));
		}
		else {
			SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), AimHit.TraceEnd));
		}
	}
	else {
		SetRelativeRotation(FRotator::ZeroRotator);
	}
	
	Character->WeaponGripLocation = GetSocketLocation("GripSocket");
	SetCrosshairSize(CurrentThreshold);


}

void UWeaponComponent::SetCrosshairSize(float ThresholdToSet) 
{
	if (Character == nullptr) { return; }
	if (Character->InGameHUD == nullptr) { return; }

	UInGameHUD *HUD = Character->InGameHUD;
	UCanvasPanelSlot* TopSlot = Cast<UCanvasPanelSlot>(HUD->TopHair->Slot);
	UCanvasPanelSlot* BottomSlot = Cast<UCanvasPanelSlot>(HUD->BottomHair->Slot);
	UCanvasPanelSlot* LeftSlot = Cast<UCanvasPanelSlot>(HUD->LeftHair->Slot);
	UCanvasPanelSlot* RightSlot = Cast<UCanvasPanelSlot>(HUD->RightHair->Slot);

	TopSlot->SetPosition(FVector2D(0, ThresholdToSet));
	BottomSlot->SetPosition(FVector2D(0, -ThresholdToSet));
	LeftSlot->SetPosition(FVector2D(-ThresholdToSet, 0));
	RightSlot->SetPosition(FVector2D(ThresholdToSet, 0));
}

void UWeaponComponent::OnRegister() {
	Super::OnRegister();
	if (GetOwner() == nullptr) { return; }
	Character = Cast<AMainCharacter>(GetOwner());
}

void UWeaponComponent::FireWeapon() {
	if (!DoesSocketExist("Barrel")) { return; }
	FVector BarrelLoc = GetSocketLocation("Barrel");
	CurrentThreshold = CurrentThreshold + WeaponSettings.FireCrosshairIncrement;
	SetCrosshairSize(CurrentThreshold);

	if (WeaponSettings.ProjectileClass != nullptr) {
		float T = CurrentThreshold * CurrentThreshold / 5000;
		FRotator RandomRotation = FRotator(FMath::RandRange(-T, T), FMath::RandRange(-T, T), FMath::RandRange(-T, T));
		AProjectile *NewProjectile = GetWorld()->SpawnActor<AProjectile>(WeaponSettings.ProjectileClass, GetSocketLocation("Barrel"), GetSocketRotation("Barrel") + RandomRotation );
	}

	UParticleSystemComponent *NewParticle = UGameplayStatics::SpawnEmitterAttached(WeaponSettings.MuzzleFlashClass, this, NAME_None, GetSocketLocation("Barrel"), GetSocketRotation("Barrel"), EAttachLocation::KeepWorldPosition);
	NewParticle->AddRelativeRotation(FRotator(0, 0, 45));
}


