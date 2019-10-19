// Fill out your copyright notice in the Description page of Project Settings.


#include "Part.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
APart::APart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(FName("Scene"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetupAttachment(Scene);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeleMesh"));
	SkeletalMesh->SetupAttachment(Scene);
}

// Called when the game starts or when spawned
void APart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

