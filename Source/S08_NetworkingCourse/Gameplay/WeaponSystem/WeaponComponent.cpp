// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Gameplay/PlayerCharacter/MainCharacter.h"
#include "UI/InGameUI/HUD/InGameHUD.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gameplay/WeaponSystem/Projectile.h"

#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

#include "Kismet/KismetMathLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"

#include "Gameplay/PlayerCharacter/MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundBase.h"
#include "Gameplay/Droid/Droid.h"

UWeaponComponent::UWeaponComponent(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	if (Mesh != nullptr) {
		this->SetStaticMesh(Mesh);
	}
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(false);
}

void UWeaponComponent::AimGun(bool Smooth) {
	FHitResult AimHit;
	GetWorld()->LineTraceSingleByChannel(
		AimHit,
		AimLocation + AimDirection * 200,
		AimLocation + AimDirection * 1000000,
		ECollisionChannel::ECC_Visibility
	);

	if ((AimHit.ImpactPoint - AimHit.TraceStart).Size() < 0) {
		AimHit.ImpactPoint = AimHit.TraceEnd;
	}

	if (Character->IsLocallyControlled() && !Character->ReloadingWeapon && !Character->Taunting && Character->WeaponEquipped && !Character->Sprinting && CanShoot && !WeaponSettings.IsMeleeWeapon) {
		FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetSocketLocation("Barrel") - GetForwardVector() * 100, AimHit.bBlockingHit ? AimHit.ImpactPoint : AimHit.TraceEnd);
		if (Smooth) {
			SetWorldRotation(FMath::RInterpTo(GetComponentRotation(), DesiredRotation, CurrentDeltaTime, 5));
		}
		else {
			SetWorldRotation(DesiredRotation);
		}
	}
	else {
		SetRelativeRotation(FRotator::ZeroRotator);
	}
	FRotator Rotation = RelativeRotation;
	Rotation.Roll = 0;
	SetRelativeRotation(Rotation);
}

void UWeaponComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CanShoot) {
		Character->WeaponGripLocation = GetSocketLocation("GripSocket");
	}

	if (CanCauseMeleeDamage && GetWorld()->GetTimeSeconds() < TimeUntilCanMeleeDamage) {
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors, ADroid::StaticClass());

		for (AActor *Actor : OverlappingActors) {
			ADroid *HitDroid = Cast<ADroid>(Actor);
			if (HitDroid != nullptr && !HitDroid->DroidState.Dead) {
				bool FoundExistingDroid = false;
				for (ADroid* ExistingDroid : ImpactedDroids) {
					if (!FoundExistingDroid && HitDroid == ExistingDroid) {
						FoundExistingDroid = true;
					}
				}

				if (!FoundExistingDroid) {
					ImpactedDroids.Add(HitDroid);
					HitDroid->DamageDroid(ProjectileSettings.Damage, FHitResult(), nullptr);
					HitDroid->GetMesh()->AddImpulse((Character->ControlRotation.Vector()*FVector(1,1,0)).Rotation().Vector() * 8000);
				}
			}
		}
	}
	if (GetWorld()->GetTimeSeconds() > TimeUntilCanMeleeDamage && CanCauseMeleeDamage) {
		CanCauseMeleeDamage = false;
		ImpactedDroids.Empty();
	}

	CurrentDeltaTime = DeltaTime;

	if (!Character->FireAnimPlaying) {
		AimGun(true);
	}

	if (!CanShoot) { return; }
	if (Character == nullptr) { return; }

	TargetThreshold = Character->Aiming ? WeaponSettings.AimIdleCrosshairThreshold : WeaponSettings.HipIdleCrosshairThreshold;

	if (!Character->Idle) {
		TargetThreshold += Character->Aiming ? WeaponSettings.WalkCrosshairIncrement : WeaponSettings.WalkCrosshairIncrement * 2;
		if (Character->Sprinting) {
			TargetThreshold += 200;
		}
	}

	CurrentThreshold = FMath::FInterpTo(CurrentThreshold, TargetThreshold, DeltaTime, WeaponSettings.CrosshairAimSpeed);

	if (Character->IsLocallyControlled()) {
		SetCrosshairSize(CurrentThreshold);
	}

	WeaponState.CurrentMagazineAmmo = CurrentMagazineAmmo;
	WeaponState.CurrentMaxAmmo = CurrentMaxAmmo;
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
	if (GetOwnerRole() == ROLE_AutonomousProxy || (GetOwnerRole() == ROLE_Authority && Cast<AMainCharacter>(GetOwner())->IsLocallyControlled())) {
		if (!CanShoot) { return; }
		if (!DoesSocketExist("Barrel")) { return; }
		if (Character == nullptr) { return; }
		if (!WeaponSettings.IsMeleeWeapon && WeaponSettings.ProjectileClass == nullptr) { return; }

		if (CurrentMagazineAmmo <= 0) {
			Character->FiringWeapon = false;
			Character->StartReloadingWeapon();
			return;
		}

		AimGun(false);

		CurrentMagazineAmmo -= 1;
		CurrentMagazineAmmo = FMath::Clamp(CurrentMagazineAmmo, 0, WeaponSettings.MagazineCapacity);

		for (int i = 0; i < WeaponSettings.ProjectilesPerShot; i++)
		{
			if (WeaponSettings.ProjectileClass != nullptr) {
				float T = CurrentThreshold * CurrentThreshold / 5000;
				FRotator RandomRotation = FRotator(FMath::RandRange(-T, T), FMath::RandRange(-T, T), FMath::RandRange(-T, T));
				Character->SpawnProjectile(FTransform(GetSocketRotation("Barrel") + RandomRotation, GetSocketLocation("Barrel")), WeaponSettings.ProjectileClass, ProjectileSettings);
				Character->ServerSpawnProjectile(FTransform(GetSocketRotation("Barrel") + RandomRotation, GetSocketLocation("Barrel")), WeaponSettings.ProjectileClass, ProjectileSettings);
			}
		}
		Character->CurrentCameraFOV = (Character->CurrentCameraFOV + WeaponSettings.CameraFOVIncrement);
		Character->TargetCameraRotation += FRotator(WeaponSettings.CameraPitchIncrement,0,0);
		Character->UpdateAmmoDisplay(CurrentMagazineAmmo, CurrentMaxAmmo);
		
		float d = WeaponSettings.CameraSwayMaxDistance;
		float a = WeaponSettings.CameraSwayMaxAngle;

		Character->TargetCameraRelativeLocation += FVector(FMath::RandRange(-d,d), FMath::RandRange(-d, d), FMath::RandRange(-d, d));
		Character->TargetCameraRotation += FRotator(FMath::RandRange(0.0f, a) * CurrentThreshold/50, FMath::RandRange(-a, a) * CurrentThreshold/50, FMath::RandRange(-a, a) * CurrentThreshold/50);

		if (!WeaponSettings.IsMeleeWeapon && WeaponSettings.MuzzleFlashClass == nullptr) { return; }
		UParticleSystemComponent *NewParticle = UGameplayStatics::SpawnEmitterAttached(WeaponSettings.MuzzleFlashClass, this, NAME_None, GetSocketLocation("Barrel"), GetSocketRotation("Barrel"), EAttachLocation::KeepWorldPosition);
		NewParticle->AddRelativeRotation(FRotator(0, 0, WeaponSettings.MuzzleAngle + CurrentMuzzleAngle));
		CurrentMuzzleAngle += WeaponSettings.MuzzleSpinRate;

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSettings.FireSound, GetSocketLocation("Barrel"), 1, WeaponSettings.BasePitch + FMath::RandRange(-0.05f, 0.05f));
		FVector BarrelLoc = GetSocketLocation("Barrel");
		CurrentThreshold = CurrentThreshold + WeaponSettings.FireCrosshairIncrement;
		SetCrosshairSize(CurrentThreshold);
	}
}
void UWeaponComponent::ReloadWeapon(bool RefilAmmo) {
	if (RefilAmmo) {
		CurrentMaxAmmo = WeaponSettings.MaxAmmo;
		CurrentMagazineAmmo = WeaponSettings.MagazineCapacity;
		Character->UpdateAmmoDisplay(CurrentMagazineAmmo, CurrentMaxAmmo);
		return;
	}
	int AmmoPendingToAdd = (WeaponSettings.MagazineCapacity - CurrentMagazineAmmo);
	if ((CurrentMaxAmmo - AmmoPendingToAdd) < 0) {
		AmmoPendingToAdd = CurrentMaxAmmo;
		CurrentMagazineAmmo += CurrentMaxAmmo;
		CurrentMaxAmmo = 0;
	}
	else {
		CurrentMaxAmmo -= (WeaponSettings.MagazineCapacity - CurrentMagazineAmmo);
		CurrentMagazineAmmo += AmmoPendingToAdd;
	}
}

void UWeaponComponent::SetWeaponState(FWeaponState State) {
	CurrentMagazineAmmo = State.CurrentMagazineAmmo;
	CurrentMaxAmmo = State.CurrentMaxAmmo;
}





