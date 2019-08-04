// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"


// Sets default values
AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableObject::Interact(AMainCharacter * Character)
{
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(100, 10, FColor::Orange, FString("INTERACTEDEDED"));
}


