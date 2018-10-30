// Fill out your copyright notice in the Description page of Project Settings.

#include "PressurePlate.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "MovingObject.h"

// Sets default values
APressurePlate::APressurePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(FName("Scene"), false);
	Scene->SetupAttachment(GetRootComponent());
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("PlateTrigger"), false);
	BoxComp->SetupAttachment(Scene);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);
	Mesh->SetupAttachment(Scene);
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APressurePlate::TriggerOverlap()
{
	if (PlatformToMove != nullptr) {
		PlatformToMove->Moving = true;
	}
}
void APressurePlate::TriggerOverlapEnd()
{
	if (PlatformToMove != nullptr) {
		PlatformToMove->Moving = false;
	}
}



