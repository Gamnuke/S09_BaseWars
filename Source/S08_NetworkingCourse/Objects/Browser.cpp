// Fill out your copyright notice in the Description page of Project Settings.

#include "Browser.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Gameplay/PlayerCharacter/MainCharacter.h"
#include "UnrealNetwork.h"

// Sets default values
ABrowser::ABrowser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TabletBase = CreateDefaultSubobject<UStaticMeshComponent>(FName("Tablet Base"));
	TabletBase->SetupAttachment(GetRootComponent());

	ScreenUI = CreateDefaultSubobject<UWidgetComponent>(FName("Screen"));
	ScreenUI->SetupAttachment(TabletBase);
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABrowser::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABrowser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABrowser::ExitFocus()
{
}

void ABrowser::Interact(class AMainCharacter *Character) {
	Super::Interact(Character);
	Character->FocusedBrowser = this;
	Interacted(Character);
}

void ABrowser::Interacted_Implementation(class AMainCharacter *Character) {
}


