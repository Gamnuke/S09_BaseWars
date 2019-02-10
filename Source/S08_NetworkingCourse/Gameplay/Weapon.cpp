// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UnrealNetwork.h"

AWeapon::AWeapon() {
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);

	Mesh->SetIsReplicated(true);
	SetReplicates(true);
	SetReplicateMovement(true);
	NetUpdateFrequency = 5;
}

void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, AssignedWeaponState);
	DOREPLIFETIME(AWeapon, AssignedWeapon);
	DOREPLIFETIME(AWeapon, SelfInitialize);
}

void AWeapon::BeginPlay() {
	Super::BeginPlay();
	if (AssignedWeapon == nullptr || !SelfInitialize) { return; }
	Mesh->SetStaticMesh(AssignedWeapon.GetDefaultObject()->GetStaticMesh());

	FWeaponState InitWeaponState;
	InitWeaponState.CurrentMagazineAmmo = AssignedWeapon.GetDefaultObject()->WeaponSettings.MagazineCapacity;
	InitWeaponState.CurrentMaxAmmo = AssignedWeapon.GetDefaultObject()->WeaponSettings.MaxAmmo;
	AssignedWeaponState = InitWeaponState;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
