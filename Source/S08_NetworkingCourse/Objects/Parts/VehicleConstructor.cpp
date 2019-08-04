// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleConstructor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Objects/Parts/Part.h"
#include "Engine/StaticMesh.h "

// Sets default values
AVehicleConstructor::AVehicleConstructor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*PartImage = CreateDefaultSubobject<UStaticMeshComponent>(FName("PartImage"));
	PartImage->SetupAttachment(GetRootComponent());*/
}

// Called when the game starts or when spawned
void AVehicleConstructor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVehicleConstructor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UInstancedStaticMeshComponent *AVehicleConstructor::CreateMesh(TSubclassOf<APart> SelectedPart) {
	UStaticMesh *MeshToCreate = SelectedPart.GetDefaultObject()->Mesh->GetStaticMesh();
	//UE_LOG(LogTemp, Warning, TEXT("Creating Mesh!!!"));
	for (UInstancedStaticMeshComponent *ExistingMesh : InstancedMeshes) {
		if (ExistingMesh->GetStaticMesh() == MeshToCreate) {
			return ExistingMesh;
			break;
		}
	}
	UInstancedStaticMeshComponent *NewMesh = NewObject<UInstancedStaticMeshComponent>(this, FName(*MeshToCreate->GetName()));
	UMaterialInterface * Material = SelectedPart.GetDefaultObject()->Mesh->GetMaterial(0);

	if (Material != nullptr) {
		NewMesh->SetMaterial(0, Material);
	}

	NewMesh->RegisterComponent();
	NewMesh->SetStaticMesh(MeshToCreate);
	NewMesh->SetupAttachment(GetRootComponent());
	InstancedMeshes.Add(NewMesh);

	
	return NewMesh;
}

