// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/StaticMeshComponent.h"

AWeapon::AWeapon() {
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);

}

void AWeapon::BeginPlay() {
	Super::BeginPlay();
}



