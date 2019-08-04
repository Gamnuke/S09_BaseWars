// Fill out your copyright notice in the Description page of Project Settings.

#include "AmmoBox.h"
#include "Components/StaticMeshComponent.h"
#include "Gameplay/PlayerCharacter/MainCharacter.h"

AAmmoBox::AAmmoBox() {
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
}
void AAmmoBox::Interact(AMainCharacter * Character) {
	Super::Interact(Character);
	if (Character != nullptr && Character->CurrentWeapon != nullptr && !Character->ReloadingWeapon && (Character->CurrentWeapon->CurrentMaxAmmo != Character->CurrentWeapon->WeaponSettings.MaxAmmo || Character->CurrentWeapon->CurrentMagazineAmmo != Character->CurrentWeapon->WeaponSettings.MagazineCapacity)) {
		Character->RefilAmmo = true;
		Character->StartReloadingWeapon();
	}
}
