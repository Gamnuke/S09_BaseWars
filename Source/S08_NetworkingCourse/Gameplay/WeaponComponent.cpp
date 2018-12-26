// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Gameplay/MainCharacter.h"
#include "MenuSystem/InGameUI/InGameHUD.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"

UWeaponComponent::UWeaponComponent(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	if (Mesh != nullptr) {
		this->SetStaticMesh(Mesh);
	}
	this->SetupAttachment(GetAttachmentRoot(), FName("Handle"));
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CurrentThreshold = FMath::FInterpTo(CurrentThreshold, TargetThreshold, DeltaTime, 20);

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
	TargetThreshold = WeaponSettings.HipIdleCrosshairThreshold;
}

void UWeaponComponent::FireWeapon() {
	if (!DoesSocketExist("Barrel")) { return; }
	FVector BarrelLoc = GetSocketLocation("Barrel");
	DrawDebugLine(GetWorld(), BarrelLoc, BarrelLoc + GetSocketRotation("Barrel").Vector() * 1000, FColor::Cyan, false, 0.1f, 0,3);
	CurrentThreshold = TargetThreshold + WeaponSettings.FireCrosshairIncrement;
	SetCrosshairSize(CurrentThreshold);
}


