// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleConstructor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/LineBatchComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
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
	SetActorLocation(FVector::ZeroVector);
	SetActorRotation(FRotator::ZeroRotator);

	for (UBoxComponent *C : CreatedBoxes) {
		C->DestroyComponent();
	}
	CreatedBoxes.Empty();
	for (UInstancedStaticMeshComponent *C : CreatedMeshes) {
		C->DestroyComponent();
	}
	CreatedMeshes.Empty();
	FVehicleData Data;
	MenuRef->LoadVehicleData(MenuRef->LoadedVehiclePath, Data, true);
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

	int y = 0;
	FColor NewRandomColor;
	if (MenuRef != nullptr && MenuRef->LoadedVehiclePath != FString()) {
		for (UInstancedStaticMeshComponent *MeshToHide : InstancedMeshes) {
			if (MeshToHide != nullptr) {
				MeshToHide->DestroyComponent();
			}
		}

		FVehicleData LoadedData;
		MenuRef->LoadVehicleData(MenuRef->LoadedVehiclePath, LoadedData, false);
		UPlatformerGameInstance *GI = MenuRef->GI;
		if (GI == nullptr) { return; }
		if (!MenuRef->CockpitLocation.IsSet()) { return; }


		FVector CockpitLocation = MenuRef->CockpitLocation.GetValue();
		TMap<FVector, TArray<FVector>> Hierachy = MenuRef->ParentChildHierachy;
		TMap<FVector, UBoxComponent*> BoxCollisions;
		TMap<FVector, UInstancedStaticMeshComponent*> Meshes;
		//TODO Make a better system for detecting where the cockpit is through raw binary data.

		int32 n_child = 0;
		int32 n_parent = 0;
		int32 n_structure = 0;
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
			RoundStruct(PartTransform, 50);

			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(o, 1000.0f, FColor::Cyan, FString(":::") + FoundPartName);
			TSubclassOf<APart> *Part = GI->NameForPart.Find(FoundPartName);
			UStaticMesh *MeshGeo = Part->GetDefaultObject()->Mesh->GetStaticMesh();
			FBox Extent = MeshGeo->GetBoundingBox();
			RoundStruct(Extent, 50);
			
			UBoxComponent *ParentBox = (BoxCollisions.FindRef(Pair.Key));
			UInstancedStaticMeshComponent *ParentMesh = (Meshes.FindRef(Pair.Key));
			FString ParentName;
			bool WasParentNull;

			if (ParentBox == nullptr) {
				ParentBox = NewObject<UBoxComponent>(this, FName(*(FString("parentBox_") + FString::FromInt(n_parent))));
				ParentMesh = NewObject<UInstancedStaticMeshComponent>(this, FName(*(FString("parentMesh_") + FString::FromInt(n_parent))));
				
				ParentMesh->SetupAttachment(ParentBox);
				ParentMesh->AddRelativeLocation(-(Extent.GetCenter()));
				ParentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				CreatedMeshes.Add(ParentMesh);

				FormatBoxCollision(*ParentBox, true);
				CreatedBoxes.Add(ParentBox);
				BoxCollisions.Add(Pair.Key, ParentBox);
				Meshes.Add(Pair.Key, ParentMesh);
				SimulatedMovables.Add(ParentBox);

				//CreateCollision(n_collision, MeshGeo, Parent, PartTransform.GetRotation().RotateVector(Extent.GetCenter()), TOptional<FRotator>());
				n_collision++;
				WasParentNull = true;
			}
			if (ParentBox != nullptr) {
				ParentName = ParentBox->GetName();
				if (Pair.Key == CockpitLocation) {
					ParentBox->SetupAttachment(BoxComp);
				}

				if (WasParentNull && MeshGeo != nullptr) {
					ParentMesh->SetStaticMesh(MeshGeo);
					ParentBox->RegisterComponent();
					ParentMesh->RegisterComponent();
				}

				ParentMesh->AddInstance(FTransform(FVector(0, 0, 0)));
				ParentBox->SetWorldTransform(PartTransform);
				ParentBox->AddWorldOffset(PartTransform.GetRotation().RotateVector(Extent.GetCenter()));
				ParentBox->SetBoxExtent(Extent.GetExtent());

				if (InstantiatedMovableParts.Find(Pair.Key) == INDEX_NONE) {
					InstantiatedMovableParts.Add(Pair.Key);
					CreateMainStructure(LoadedData, Pair.Key, MovableParts, GI, n_structure, ParentBox, n_collision);
				//	DrawDebugPoint(GetWorld(), Pair.Key, 20, FColor::Purple, false, 1000, 72);
				}
				//CreateCollision(n_collision, MeshGeo, Parent, PartTransform);
				//Comp->SetMassOverrideInKg(NAME_None, 10, true);
			}

			//---Make Children---//
			for (FVector ChildLoc : Pair.Value) {
				UBoxComponent *ChildBox = (BoxCollisions.FindRef(ChildLoc));
				UInstancedStaticMeshComponent *ChildMesh = (Meshes.FindRef(ChildLoc));
				bool WasChildNull = false;

				FTransform PartTransform;
				FString FoundPartName = GetPartNameFromLocation(ChildLoc, LoadedData.PartData, PartTransform);
				FString ChildName;
				RoundStruct(PartTransform, 50);

				TSubclassOf<APart> *Part = GI->NameForPart.Find(FoundPartName);
				if (Part == nullptr) {
					DebugMessage(FString("Part is null!"));
					return;
				}
				else if (Part->GetDefaultObject() == nullptr) {
					DebugMessage(FString("Default object is null!"));
					return;
				}
				else if (Part->GetDefaultObject()->Mesh == nullptr) {
					DebugMessage(FString("Mesh component is null!"));
					return;
				}
				else if (Part->GetDefaultObject()->Mesh->GetStaticMesh() == nullptr) {
					DebugMessage(FString("Static Mesh is null!"));
					return;

				}
				UStaticMesh *MeshGeo = Part->GetDefaultObject()->Mesh->GetStaticMesh();

				if (ChildBox == nullptr) {
					ChildBox = NewObject<UBoxComponent>(this, FName(*(FString("childBox_") + FString::FromInt(n_child))));
					ChildMesh = NewObject<UInstancedStaticMeshComponent>(this, FName(*(FString("childMesh_") + FString::FromInt(n_child))));
					ChildMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					ChildMesh->SetupAttachment(ChildBox);
					ChildMesh->RegisterComponent();
					CreatedMeshes.Add(ChildMesh);
					if (MeshGeo != nullptr) {
						ChildMesh->SetStaticMesh(MeshGeo);
					}

					FBox Extent = MeshGeo->GetBoundingBox();
					RoundStruct(Extent, 50);
					ChildBox->AddRelativeLocation(-(Extent.GetCenter()));
					ChildBox->SetBoxExtent(Extent.GetExtent());
					ChildBox->bHiddenInGame = false;
					ChildBox->SetVisibility(true);
					FormatBoxCollision(*ChildBox, true);

					CreatedBoxes.Add(ChildBox);
					BoxCollisions.Add(ChildLoc, ChildBox);
					Meshes.Add(ChildLoc, ChildMesh);

					SimulatedMovables.Add(ChildBox);
					//CreateCollision(n_collision, MeshGeo, Child, PartTransform.GetRotation().RotateVector(Extent.GetCenter()), TOptional<FRotator>());

					WasChildNull = true;
				}
				if (ChildBox != nullptr) {

					//if (Part == nullptr) { GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(58, 20, FColor::Red, FString("COULDNT FIND PART!!!!! :("));  return; }
					ChildName = ChildBox->GetName();
					if (WasChildNull) {
						ChildBox->RegisterComponent();
						ChildMesh->AddInstance(FTransform(FVector(0, 0, 0)));
					}

					ChildBox->SetWorldTransform(PartTransform);
					ChildBox->AttachToComponent(ParentBox, FAttachmentTransformRules::KeepWorldTransform);
					//ChildBox->AddWorldOffset(PartTransform.GetRotation().RotateVector(Extent.GetCenter()));

					if (InstantiatedMovableParts.Find(ChildLoc) == INDEX_NONE) {
						InstantiatedMovableParts.Add(ChildLoc);
						CreateMainStructure(LoadedData, ChildLoc, MovableParts, GI, n_structure, ChildBox, n_collision);
					}
				}

				//Create the constraints
				if (Part->GetDefaultObject()->Details.IsMovable) {
					UPhysicsConstraintComponent *PhysCont = NewObject<UPhysicsConstraintComponent>(this, FName(*(FString("physConstraint_") + FString::FromInt(n_child))));
					PhysCont->SetupAttachment(ParentBox);
					PhysCont->ComponentName1.ComponentName = FName(*ChildName);
					PhysCont->ComponentName2.ComponentName = FName(*ParentName);
					PhysCont->SetDisableCollision(true);
					PhysCont->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
					PhysCont->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);
					PhysCont->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0);


					FRotator PR = PartTransform.GetRotation().Rotator();
					FRotator DesiredRot = FRotator(0, PR.Yaw, 0);
					PhysCont->SetRelativeRotation(FRotator(0, 0, -PR.Roll));

					//if (PR.Vector() == FVector::UpVector) {
						PhysCont->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 0);
						PhysCont->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
						PhysCont->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
						//}
					PhysCont->SetWorldLocation(PartTransform.GetLocation());

					PhysCont->RegisterComponent();
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

void AVehicleConstructor::RoundStruct(FBox &Extent, int32 RoundTo) {
	int32 r = RoundTo;
	FVector E = Extent.GetExtent();
	E.X = FMath::RoundToInt(E.X / r) * r;
	E.Y = FMath::RoundToInt(E.Y / r) * r;
	E.Z = FMath::RoundToInt(E.Z / r) * r;

	FVector C = Extent.GetCenter();
	C.X = FMath::RoundToInt(C.X / r) * r;
	C.Y = FMath::RoundToInt(C.Y / r) * r;
	C.Z = FMath::RoundToInt(C.Z / r) * r;

	Extent = Extent.BuildAABB(C, E);
}
void AVehicleConstructor::RoundStruct(FTransform &Transform, int32 RoundTo) {
	int32 r = RoundTo;
	FVector E = Transform.GetLocation();
	E.X = FMath::RoundToInt(E.X / r) * r;
	E.Y = FMath::RoundToInt(E.Y / r) * r;
	E.Z = FMath::RoundToInt(E.Z / r) * r;

	FRotator C = Transform.GetRotation().Rotator();
	C.Pitch = FMath::RoundToInt(C.Pitch / 90) * 90;
	C.Yaw = FMath::RoundToInt(C.Yaw / 90) * 90;
	C.Roll = FMath::RoundToInt(C.Roll / 90) * 90;

	Transform = FTransform(C, E);
}

void AVehicleConstructor::FormatBoxCollision(UBoxComponent &Box, bool bSimulatePhysics) {
	Box.SetSimulatePhysics(bSimulatePhysics);
	Box.SetVisibility(true);
	Box.bHiddenInGame = false;
	Box.SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);
	Box.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box.SetCollisionProfileName("BlockAll");
}

// Called when the game starts or when spawned
void AVehicleConstructor::BeginPlay()
{
	Super::BeginPlay();

	if (TEST.IsSet()) {
		DebugMessage(FString("TEST IS SET"));
	}
	else {
		DebugMessage(FString("TEST IS NOOOT SET"));
	}
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
		if (Mesh != nullptr) {
			Mesh->DestroyComponent();
		}
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

void AVehicleConstructor::CreateMainStructure(FVehicleData &LoadedData, FVector &ChildLoc, FOccluderVertexArray &MovableParts, UPlatformerGameInstance * GI, int32 &n_structure, UBoxComponent * ParentBoxPtr, int32 &n_collision)
{
	//order++;
	// Get the parts connecting to this parent
	TArray<FVector> PartsToScan = LoadedData.WeldedParts.FindRef(ChildLoc);
	FVector *RootPart = MenuRef->MovablePartToRoot.Find(ChildLoc);

	

	if (RootPart != nullptr) {
		PartsToScan.Remove(*RootPart);
	}

	// Remove the root part if it was found
	TArray<UInstancedStaticMeshComponent*> StructureMeshes;

	// Check if those initial parts to scan are movable parts and remove them to filter only the static parts
	for (FVector PartToOmit : MovableParts) {
		PartsToScan.Remove(PartToOmit);
	}
	//PartsToScan.Remove(MenuRef->CockpitLocation.GetValue());

	TArray<FVector> ScannedParts;
	ScannedParts = MovableParts;
	ScannedParts.Add(MenuRef->CockpitLocation.GetValue());

	while (PartsToScan.Num() > 0)
	{

		for (FVector ScanningPart : PartsToScan) {
			MenuRef->RoundVector(ScanningPart);
			PartsToScan.Remove(ScanningPart); // not sure if this should be at the end
			ScannedParts.Add(ScanningPart);
			//DrawDebugPoint(GetWorld(), ScanningPart, 10, FColor::Orange, false, 10, 72);

			////GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(y, 20, FColor::Red, FString("Parts left to scan: ") + FString::FromInt(y));

			TArray<FVector> PartsSurroundingScanned = LoadedData.WeldedParts.FindRef(ScanningPart);
			for (FVector PartToAdd : PartsSurroundingScanned) {
				if (ScannedParts.Find(PartToAdd) == INDEX_NONE) {
					PartsToScan.Add(PartToAdd);
				}
			}
			//for (FVector PartToBuffer : PartsSurroundingScanned) {

			//	// If the part that we're buffering is not a movable part and if it hasn't already been scanned...
			//	if (ScannedParts.Find(PartToBuffer) == INDEX_NONE && MovableParts.Find(PartToBuffer) == INDEX_NONE && PartsToScan.Find(PartToBuffer) == INDEX_NONE && PartToBuffer != MenuRef->CockpitLocation.GetValue()) {
			//		PartsToScan.Add(PartToBuffer);
			//	}
			//}
			/*for (FVector MovablePart : MovableParts) {
				PartsSurroundingScanned.Remove(MovablePart);
			}
			for (FVector AlreadyScannedPart : ScannedParts) {
				PartsSurroundingScanned.Remove(AlreadyScannedPart);
			}
			PartsToScan.Append(PartsSurroundingScanned);*/


			FTransform PartTransform;
			FString FoundPartName = GetPartNameFromLocation(ScanningPart, LoadedData.PartData, PartTransform);
			RoundStruct(PartTransform, 50);

			TSubclassOf<APart> *Part = GI->NameForPart.Find(FoundPartName);
			//if (Part == nullptr) { DebugMessage(FString("Couldnt find structural part: ") + FString(FoundPartName) + FString(" with transform: ") + ScanningPart.ToString()); return; }
			if (Part != nullptr) {

				if (Part->GetDefaultObject() == nullptr) {
					DebugMessage(FString("Default object is null!"));
					return;
				}
				else if (Part->GetDefaultObject()->Mesh == nullptr) {
					DebugMessage(FString("Mesh component is null!"));
					return;
				}
				else if (Part->GetDefaultObject()->Mesh->GetStaticMesh() == nullptr) {
					DebugMessage(FString("Static Mesh is null!"));
					return;

				}
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
					ExistingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					ExistingMesh->RegisterComponent();
					ExistingMesh->SetStaticMesh(MeshGeo);
					StructureMeshes.Add(ExistingMesh);
					ExistingMesh->AttachToComponent(ParentBoxPtr, FAttachmentTransformRules::KeepWorldTransform);
					CreatedMeshes.Add(ExistingMesh);
				}

				ExistingMesh->AddInstance(PartTransform);
				FBox Extent = MeshGeo->GetBoundingBox();
				RoundStruct(Extent, 50);

				UBoxComponent *BoxCol = NewObject<UBoxComponent>(this, FName(*(FString("structureCollision_") + FString::FromInt(n_collision))));
				BoxCol->SetVisibility(true);
				BoxCol->bHiddenInGame = false; BoxCol->SetBoxExtent(Extent.GetExtent());
				BoxCol->AttachToComponent(ParentBoxPtr, FAttachmentTransformRules::KeepWorldTransform);
				BoxCol->SetWorldTransform(PartTransform);
				BoxCol->AddWorldOffset(PartTransform.GetRotation().RotateVector(Extent.GetCenter()));
				CreatedBoxes.Add(BoxCol);
				FormatBoxCollision(*BoxCol, false);
				BoxCol->RegisterComponent();
				//CreateCollision(n_collision, MeshGeo, Child, PartTransform.GetLocation() + PartTransform.GetRotation().RotateVector(Extent.GetCenter()), PartTransform.GetRotation().Rotator());
				n_collision++;
				// Buffer the parts around it


			}
		}
	}
}
