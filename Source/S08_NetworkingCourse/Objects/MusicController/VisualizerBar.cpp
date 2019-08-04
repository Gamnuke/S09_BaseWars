// Fill out your copyright notice in the Description page of Project Settings.

#include "VisualizerBar.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AVisualizerBar::AVisualizerBar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(Root);
	BarMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BarMesh"));
	BarMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AVisualizerBar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVisualizerBar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

