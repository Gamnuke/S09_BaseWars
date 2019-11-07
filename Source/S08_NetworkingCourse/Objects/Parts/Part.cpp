// Fill out your copyright notice in the Description page of Project Settings.


#include "Part.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
UPart::UPart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	/*Scene = CreateDefaultSubobject<USceneComponent>(FName("Scene"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetupAttachment(Scene);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeleMesh"));
	SkeletalMesh->SetupAttachment(Scene);*/

}

// Called when the game starts or when spawned
void UPart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void UPart::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPart::OnSelected()
{
}

void UPart::AllowVariableEdit(float & VariableToEdit, FVector2D Limits)
{
	// Create a slider to edit a variable on the Part.
}

void UPart::AllowVariableEdit(int32 & VariableToEdit, FVector2D Limits)
{
	// Create a slider to edit a variable on the Part.
}

void UPart::AllowVariableEdit(FString & VariableToEdit, int32 CharacterLimit)
{
	// Create a slider to edit a variable on the Part.
}

