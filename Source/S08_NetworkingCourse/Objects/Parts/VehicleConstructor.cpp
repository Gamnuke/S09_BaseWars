// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleConstructor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/LineBatchComponent.h"
#include "Components/BoxComponent.h"
#include "Objects/Parts/Part.h"
#include "Engine/StaticMesh.h"
#include "PhysicsEngine/BodySetup.h"
#include "UI/NetworkUI/Menu.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AVehicleConstructor::AVehicleConstructor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*PartImage = CreateDefaultSubobject<UStaticMeshComponent>(FName("PartImage"));
	PartImage->SetupAttachment(GetRootComponent());*/

	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	SetRootComponent(BoxComp);

	LineComponent = CreateDefaultSubobject<ULineBatchComponent>(FName("LineDrawComponent"));
	LineComponent->SetupAttachment(BoxComp);


}
bool AVehicleConstructor::SetSimulation(bool bSimulateVehicle)
{
	bSimulateVehicle ? BuildSimulatedVehicle() : ReverseSimulation();
	bSimulatingVehicle = bSimulateVehicle;
	SetGates(bSimulateVehicle);
	return true;
}
void AVehicleConstructor::ReverseSimulation() {
	BoxComp->SetSimulatePhysics(false);
	BoxComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	for (UBoxComponent *C : PartialBoxComps) {
		C->DestroyComponent();
	}
	PartialBoxComps.Empty();
	SetActorLocation(FVector::ZeroVector);
	SetActorRotation(FRotator::ZeroRotator);
}

void AVehicleConstructor::SetGates_Implementation(bool bGateState)
{
}

FTransform AVehicleConstructor::FindTransformFromLocation(FVector Location, TArray<FTransform> Transforms) {
	for (FTransform T : Transforms) {
		if (T.GetLocation() == Location) {
			return T;
		}
	}
	return FTransform();
}

FString AVehicleConstructor::GetPartNameFromLocation(FVector LocationToSearch, TMap<FString, TArray<FTransform>> PartForName, FTransform &FoundTransform) {
	//DebugMessage(FString("Oh hey, just about to go to bed. I know we couldn't skype tonight, but, thats alright. Goodnight girl, i'll see you tomorrow"));
	for (TPair<FString, TArray<FTransform>> Pair : PartForName) {
		// Given the location of the part we want, we first find the entire transform of that part in FindTransformFromLocation, then we
		// find the part name by the transform.

		FTransform SearchTransform = FindTransformFromLocation(LocationToSearch, Pair.Value);
		for (FTransform T : Pair.Value) {
			if (T.GetLocation() == SearchTransform.GetLocation()) {
				FoundTransform = T;
				return Pair.Key;
			}
		}
		/*if (Pair.Value.Find(SearchTransform) != INDEX_NONE) {
			FoundTransform = SearchTransform;
			return Pair.Key;
		}*/
	}
	return FString();

}

void AVehicleConstructor::DebugMessage(FString Message) {
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(MessageIndex, 1000.0f, FColor::MakeRandomColor(), FString::FromInt(MessageIndex-70) + FString(" | ") + Message);
	MessageIndex++;
}


void AVehicleConstructor::BuildSimulatedVehicle()
{
	int32 n_structure = 0;
	int y = 0;
	FColor NewRandomColor;
	if (MenuRef != nullptr && MenuRef->LoadedVehiclePath != FString()) {
		for (UInstancedStaticMeshComponent *MeshToHide : InstancedMeshes) {
			MeshToHide->DestroyComponent();
		}

		FVehicleData LoadedData;
		MenuRef->LoadVehicleData(MenuRef->LoadedVehiclePath, LoadedData, false);
		UPlatformerGameInstance *GI = MenuRef->GI;
		if (GI == nullptr) { return; }
		if (!MenuRef->CockpitLocation.IsSet()) { return; }

		TMap<FVector, UInstancedStaticMeshComponent*> Meshes;

		FVector CockpitLocation = MenuRef->CockpitLocation.GetValue();
		TMap<FVector, TArray<FVector>> Hierachy = MenuRef->ParentChildHierachy;
		//TODO Make a better system for detecting where the cockpit is through raw binary data.

		int32 n_child = 0;
		int32 n_parent = 0;
		int32 n_collision = 0;

		TArray<FVector> MovableParts; //Gets an array of the location of only the movable parts.
		for (TPair<FVector, TArray<FVector>> Pair : Hierachy) {
			if (MovableParts.Find(Pair.Key) == INDEX_NONE) {
				MovableParts.Add(Pair.Key);
			}
			for (FVector k : Pair.Value) {
				if (MovableParts.Find(k) == INDEX_NONE) {
					MovableParts.Add(k);
				}
			}
		}

		TArray<FVector> InstantiatedMovableParts; //the movable parts that we already created the main structure for
		int32 o = 0;
		//--- Create the movable parts
		for (TPair<FVector, TArray<FVector>> Pair : MenuRef->ParentChildHierachy) {
			NewRandomColor = FColor::MakeRandomColor();
			o++;
			//Check if the parent exists
			// If it doesn't, then create it.
			// For each of its children,
			// Check if the child exists. If it does, get the instance and parent it to the created/found parent.
			// If it doesn't, create the child and parent it to the created/found parent.

			//---Make Parent---//
			FTransform PartTransform;
			FString FoundPartName = GetPartNameFromLocation(Pair.Key, LoadedData.PartData, PartTransform);
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(o, 1000.0f, FColor::Cyan, FString(":::") + FoundPartName);
			TSubclassOf<APart> *Part = GI->NameForPart.Find(FoundPartName);
			UStaticMesh *MeshGeo = Part->GetDefaultObject()->Mesh->GetStaticMesh();

			UInstancedStaticMeshComponent *Parent = (Meshes.FindRef(Pair.Key));
			bool WasParentNull;
			if (Parent == nullptr) {
				Parent = NewObject<UInstancedStaticMeshComponent>(this, FName(*(FString("parent_") + FString::FromInt(n_parent))));
				Meshes.Add(Pair.Key, Parent);
				SimulatedMovables.Add(Parent);

				FBox Extent = MeshGeo->GetBoundingBox();
				//CreateCollision(n_collision, MeshGeo, Parent, PartTransform.GetRotation().RotateVector(Extent.GetCenter()), TOptional<FRotator>());
				n_collision++;
				WasParentNull = true;
			}
			if (Parent != nullptr) {
				if (Pair.Key == CockpitLocation) {
					Parent->SetupAttachment(BoxComp);
				}

				if (WasParentNull && MeshGeo != nullptr) {
					Parent->SetStaticMesh(MeshGeo);
					Parent->RegisterComponent();
				}

				Parent->AddInstance(FTransform(FVector(0, 0, 0)));
				Parent->SetWorldTransform(PartTransform);

				if (InstantiatedMovableParts.Find(Pair.Key) == INDEX_NONE) {
					InstantiatedMovableParts.Add(Pair.Key);
					CreateMainStructure(LoadedData, Pair.Key, MovableParts, GI, n_structure, Parent, n_collision);
				}

				//CreateCollision(n_collision, MeshGeo, Parent, PartTransform);
				//Comp->SetMassOverrideInKg(NAME_None, 10, true);
			}

			//---Make Children---//
			for (FVector ChildLoc : Pair.Value) {
				UInstancedStaticMeshComponent *Child = (Meshes.FindRef(ChildLoc));
				bool WasChildNull = false;

				FTransform PartTransform;
				FString FoundPartName = GetPartNameFromLocation(ChildLoc, LoadedData.PartData, PartTransform);
				TSubclassOf<APart> *Part = GI->NameForPart.Find(FoundPartName);
				UStaticMesh *MeshGeo = Part->GetDefaultObject()->Mesh->GetStaticMesh();

				if (Child == nullptr) {
					Child = NewObject<UInstancedStaticMeshComponent>(this, FName(*(FString("child_") + FString::FromInt(n_child))));

					if (MeshGeo != nullptr) {
						Child->SetStaticMesh(MeshGeo);
					}


					Meshes.Add(ChildLoc, Child);
					SimulatedMovables.Add(Child);
					FBox Extent = MeshGeo->GetBoundingBox();
					//CreateCollision(n_collision, MeshGeo, Child, PartTransform.GetRotation().RotateVector(Extent.GetCenter()), TOptional<FRotator>());

					WasChildNull = true;
				}
				if (Child != nullptr) {

					//if (Part == nullptr) { GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(58, 20, FColor::Red, FString("COULDNT FIND PART!!!!! :("));  return; }

					if (WasChildNull) {
						Child->RegisterComponent();
						Child->AddInstance(FTransform(FVector(0, 0, 0)));
					}

					Child->SetWorldTransform(PartTransform);
					Child->AttachToComponent(Parent, FAttachmentTransformRules::KeepWorldTransform);
					if (InstantiatedMovableParts.Find(ChildLoc) == INDEX_NONE) {
						InstantiatedMovableParts.Add(ChildLoc);
						CreateMainStructure(LoadedData, ChildLoc, MovableParts, GI, n_structure, Child, n_collision);

					}
				}

				n_child++;
			}
		}
	}

	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	/*
	for (TPair<FString, TArray<FTransform>> PartTrans : LoadedData.PartData) {

			}*/
			//int n = 0;
			//for (UInstancedStaticMeshComponent *C : InstancedMeshes) {
			//	FBox Extent = C->GetStaticMesh()->GetBoundingBox();
			//	FVector Bound = C->GetStaticMesh()->PositiveBoundsExtension;
			//	for (int32 i = 0; i < C->GetInstanceCount(); i++)
			//	{
			//		FTransform Transform;
			//		C->GetInstanceTransform(i, Transform, false);
			//		UBoxComponent *Comp = NewObject<UBoxComponent>(this, FName(*FString::FromInt(n)));
			//		Comp->SetHiddenInGame(false);
			//		Comp->SetVisibility(true);
			//		Comp->SetupAttachment(BoxComp);
			//		Comp->SetRelativeLocation(Transform.GetLocation() + Transform.GetRotation().RotateVector(Extent.GetCenter()));
			//		Comp->SetRelativeRotation(Transform.GetRotation());
			//		Comp->SetBoxExtent(Extent.GetExtent() );
			//		Comp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
			//		Comp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			//		Comp->SetCollisionProfileName(FName("BlockAll"));
			//		Comp->SetPhysMaterialOverride(PhysMaterial);
			//		Comp->RegisterComponent();
			//		PartialBoxComps.Add(Comp);
			//		Mass += Comp->GetMass();
			//		//Comp->SetMassOverrideInKg(NAME_None, 10, true);

			//		n++;

			//	}
			//}



}

void AVehicleConstructor::CreateCollision(int32 &n_collision, UStaticMesh *MeshGeo, UInstancedStaticMeshComponent *Parent, TOptional<FVector> PartLocation, TOptional<FRotator> PartRotation) {
	FBox Extent = MeshGeo->GetBoundingBox();
	FVector Bound = MeshGeo->PositiveBoundsExtension;
	UBoxComponent *Comp = NewObject<UBoxComponent>(this, FName(*FString::FromInt(n_collision)));
	Comp->SetHiddenInGame(false);
	Comp->SetVisibility(true);
	Comp->SetupAttachment(Parent);
	Comp->WeldTo(BoxComp);

	if (PartLocation.IsSet()) { Comp->SetWorldLocation(PartLocation.GetValue()); }
	if (PartRotation.IsSet()) { Comp->SetWorldRotation(PartRotation.GetValue()); }

	Comp->SetBoxExtent(Extent.GetExtent());
	Comp->SetSimulatePhysics(false);
	Comp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Comp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Comp->SetCollisionProfileName(FName("BlockAll"));
	//Comp->SetPhysMaterialOverride(PhysMaterial);
	Comp->RegisterComponent();
	//PartialBoxComps.Add(Comp);
	//Mass += Comp->GetMass();
	n_collision++;
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

	//APlayerController *C = Cast<APlayerController>(Get);
	//if (C == nullptr) { return; }
	//FVector WorldLoc;
	//FVector Dir;
	//C->DeprojectMousePositionToWorld(WorldLoc, Dir);

	//FHitResult Hit;
	//GetWorld()->LineTraceSingleByChannel(Hit, WorldLoc, WorldLoc + Dir * 10000, ECollisionChannel::ECC_Visibility);

	////if (Hit.bBlockingHit) {
	//for (UInstancedStaticMeshComponent *Movable : SimulatedMovables) {
	//	if (Movable->GetComponentLocation() != MenuRef->CockpitLocation) {
	//		//Movable->AddLocalRotation(FRotator(0, DeltaTime * 200, 0));
	//		/*FRotator InitialRotation = Movable->RelativeRotation;
	//		FRotator LookatRotator = UKismetMathLibrary::FindLookAtRotation(Movable->GetComponentLocation(), Hit.ImpactPoint);

	//		Movable->SetWorldRotation(LookatRotator);
	//		Movable->SetRelativeRotation(FRotator(InitialRotation.Pitch, Movable->RelativeRotation.Yaw, InitialRotation.Roll));*/
	//	}
	//}
	//}


}

UInstancedStaticMeshComponent *AVehicleConstructor::CreateMesh(TSubclassOf<APart> SelectedPart) {
	UStaticMesh *MeshToCreate = SelectedPart.GetDefaultObject()->Mesh->GetStaticMesh();
	TArray<FKBoxElem> BoxElements = MeshToCreate->BodySetup->AggGeom.BoxElems;
	//InstancedMeshes[0]->GetStaticMesh()->
	//UE_LOG(LogTemp, Warning, TEXT("Creating Mesh!!!"));
	for (UInstancedStaticMeshComponent *ExistingMesh : InstancedMeshes) { // Check if an instanced mesh component of this part type exists, if so, return it to be accessed and populated by the menu.
		if (ExistingMesh->GetStaticMesh() == MeshToCreate) {
			return ExistingMesh;
			break;
		}
	}
	UInstancedStaticMeshComponent *NewMesh = NewObject<UInstancedStaticMeshComponent>(this, FName(*MeshToCreate->GetName())); // If existing mesh hasn't been found, create one.
	UMaterialInterface * Material = SelectedPart.GetDefaultObject()->Mesh->GetMaterial(0);

	NewMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (Material != nullptr) {
		NewMesh->SetMaterial(0, Material);
	}

	//NewMesh->SetupAttachment(BoxComp);
	NewMesh->AttachToComponent(BoxComp, FAttachmentTransformRules::KeepRelativeTransform);

	NewMesh->SetStaticMesh(MeshToCreate);
	NewMesh->RegisterComponent();

	PartToMesh.Add(NewMesh, SelectedPart);
	InstancedMeshes.Add(NewMesh);


	return NewMesh;
}

void AVehicleConstructor::RemoveMeshes() {
	for (UInstancedStaticMeshComponent *Mesh : InstancedMeshes) {
		Mesh->DestroyComponent();
	}
	InstancedMeshes.Empty();
	PartToMesh.Empty();
}

void AVehicleConstructor::CheckIfMeshIsEmpty(UInstancedStaticMeshComponent *MeshToCheck) {
	if (MeshToCheck->GetInstanceCount() == 0) {
		PartToMesh.Remove(MeshToCheck);
		if (MeshToCheck != nullptr) {
			InstancedMeshes.Remove(MeshToCheck);
			MeshToCheck->DestroyComponent();
		}
	}
}

void AVehicleConstructor::CreateMainStructure(FVehicleData &LoadedData, FVector &ChildLoc, FOccluderVertexArray &MovableParts, UPlatformerGameInstance * GI, int32 &n_structure, UInstancedStaticMeshComponent * Child, int32 &n_collision)
{
	//order++;
	// Get the parts connecting to this parent
	TArray<FVector> PartsToScan = LoadedData.WeldedParts.FindRef(ChildLoc);
	FVector *RootPart = MenuRef->MovablePartToRoot.Find(ChildLoc);

	// Remove the root part if it was found
	if (RootPart != nullptr) {
		PartsToScan.Remove(*RootPart);
	}

	TArray<UInstancedStaticMeshComponent*> StructureMeshes;

	// Check if those initial parts to scan are movable parts and remove them to filter only the static parts
	for (FVector PartToOmit : MovableParts) {
		if (PartsToScan.Find(PartToOmit) != INDEX_NONE) {
			PartsToScan.Remove(PartToOmit);
		}
	}

	TArray<FVector> ScannedParts;
	ScannedParts = MovableParts;

	while (PartsToScan.Num() > 0)
	{

		for (FVector ScanningPart : PartsToScan) {
			MenuRef->RoundVector(ScanningPart);
			//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(y, 20, FColor::Red, FString("Parts left to scan: ") + FString::FromInt(y));

			// Make the part //
			//Get the subclass of the part
			FTransform PartTransform;
			FString FoundPartName = GetPartNameFromLocation(ScanningPart, LoadedData.PartData, PartTransform);

			TSubclassOf<APart> *Part = GI->NameForPart.Find(FoundPartName);
			if (Part == nullptr) { GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(order, 200, FColor::Red, FString("Couldnt find structural part: ") + FString(FoundPartName) + FString(" with transform: ") + ScanningPart.ToString());  return; }

			UStaticMesh *MeshGeo = Part->GetDefaultObject()->Mesh->GetStaticMesh();

			//Check if there is already a instancedmeshcomponent that exists.
			UInstancedStaticMeshComponent *ExistingMesh = nullptr;
			for (UInstancedStaticMeshComponent *Mesh : StructureMeshes) {
				if (Mesh->GetStaticMesh() == MeshGeo) {
					ExistingMesh = Mesh;
					break;
				}
			}
			if (ExistingMesh == nullptr) {
				n_structure++;
				ExistingMesh = NewObject<UInstancedStaticMeshComponent>(this, FName(*(FString("structure_") + FString::FromInt(n_structure))));
				ExistingMesh->RegisterComponent();
				ExistingMesh->SetStaticMesh(MeshGeo);
				StructureMeshes.Add(ExistingMesh);
				ExistingMesh->AttachToComponent(Child, FAttachmentTransformRules::KeepWorldTransform);
			}

			ExistingMesh->AddInstance(PartTransform);
			FBox Extent = MeshGeo->GetBoundingBox();
			CreateCollision(n_collision, MeshGeo, Child, PartTransform.GetLocation() + PartTransform.GetRotation().RotateVector(Extent.GetCenter()), PartTransform.GetRotation().Rotator());
			n_collision++;

			// Buffer the parts around it
			TArray<FVector> *PartsSurroundingScanned = LoadedData.WeldedParts.Find(ScanningPart);
			for (FVector PartToBuffer : *PartsSurroundingScanned) {

				// If the part that we're buffering is not a movable part and if it hasn't already been scanned...
				if (ScannedParts.Find(PartToBuffer) == INDEX_NONE && MovableParts.Find(PartToBuffer) == INDEX_NONE) {
					PartsToScan.Add(PartToBuffer);
				}
			}

			ScannedParts.Add(ScanningPart);
			PartsToScan.Remove(ScanningPart); // not sure if this should be at the end

		}
	}
}
