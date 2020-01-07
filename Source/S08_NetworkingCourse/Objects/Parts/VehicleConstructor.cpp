// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleConstructor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/LineBatchComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Objects/Parts/Part.h"
#include "Engine/StaticMesh.h"
#include "PhysicsEngine/BodySetup.h"
#include "UI/NetworkUI/Menu.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "SimpleWheeledVehicleMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Gameplay/PlayerCharacter/SimulatedVehicle.h"
#include "ConstructorHelpers.h"
#include "Objects/Parts/SkeletalPartMesh.h"
#include "Objects/Parts/StaticPartMesh.h"

// Sets default values
AVehicleConstructor::AVehicleConstructor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*PartImage = CreateDefaultSubobject<UStaticMeshComponent>(FName("PartImage"));
	PartImage->SetupAttachment(GetRootComponent());*/

	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	SetRootComponent(BoxComp);

	LineComponent = CreateDefaultSubobject<ULineBatchComponent>(FName("LineDrawComponent"));
	LineComponent->SetupAttachment(BoxComp);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(BoxComp);

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);

	/*static ConstructorHelpers::FClassFinder<UUserWidget> VehicleClassSearch(TEXT("/Game/Blueprints/SimulatedVehicle_BP"));
	if (VehicleClassSearch.Class != NULL)
	{
		VehicleClass = VehicleClassSearch.Class;
	}*/
}
void AVehicleConstructor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AVehicleConstructor::SetSimulation(bool bSimulateVehicle)
{
	bSimulateVehicle ? BuildSimulatedVehicle() : ReverseSimulation();
	bSimulatingVehicle = bSimulateVehicle;
	SetGates(bSimulateVehicle);
	return true;
}
void AVehicleConstructor::ReverseSimulation() {
	/*BoxComp->SetSimulatePhysics(false);
	BoxComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
	SetActorLocation(FVector::ZeroVector);
	SetActorRotation(FRotator::ZeroRotator);

	for (UBoxComponent *C : CreatedBoxes) {
		C->DestroyComponent();
	}
	CreatedBoxes.Empty();
	for (UMeshComponent *C : CreatedMeshes) {
		C->DestroyComponent();
	}
	CreatedMeshes.Empty();
	for (UPhysicsConstraintComponent *C : HorConstraints) {
		C->DestroyComponent();
	}
	HorConstraints.Empty();
	for (UPhysicsConstraintComponent *C : VerConstraints) {
		C->DestroyComponent();
	}
	VerConstraints.Empty();
	if (CockpitBox != nullptr) {
		CockpitBox->DestroyComponent();
	}

	FVehicleData Data = FVehicleData();
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

void AVehicleConstructor::SpawnVehicle_Implementation() {

}

void AVehicleConstructor::BuildSimulatedVehicle()
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.bNoFail = true;
	ASimulatedVehicle *NewVehicle = GetWorld()->SpawnActor<ASimulatedVehicle>(VehicleClass, FVector(0,0,0), FRotator(), Params);
	if (NewVehicle == nullptr) { return; }
	FInputModeGameOnly InputMode;
	GetGameInstance()->GetFirstLocalPlayerController(GetWorld())->Possess(NewVehicle);
	GetGameInstance()->GetFirstLocalPlayerController(GetWorld())->SetInputMode(InputMode);
	//this is a test//
	/*UStaticMeshComponent *test = NewObject<UStaticMeshComponent>(this, "Yeet");
	UMeshComponent *CastTo = Cast<UMeshComponent>(test);
	if (CastTo != nullptr) {
		DebugMessage(FString("STATIC MESH COMPONENT TO MESH COMPONENT CAST SUCCESSFUL"));
		if (Cast<UStaticMeshComponent>(CastTo) != nullptr) {
			DebugMessage(FString("MESH COMPONENT TO STATIC MESH COMPONENT CAST SUCCESSFUL"));
		}
		else {
			DebugMessage(FString("MESH COMPONENT TO STATIC MESH COMPONENT CAST FAILED"));
		}

		if (Cast<USkeletalMeshComponent>(CastTo) != nullptr) {
			DebugMessage(FString("MESH COMPONENT TO SKELETAL MESH COMPONENT CAST SUCCESSFUL"));

		}
		else {
			DebugMessage(FString("MESH COMPONENT TO SKELETAL MESH COMPONENT CAST FAILED"));

		}
	}
	else {
		DebugMessage(FString("STATIC MESH COMPONENT TO MESH COMPONENT CAST FAILED"));
	}*/

	RemoveMeshes();

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
		TMap<FVector, UMeshComponent*> Meshes;
		//TODO Make a better system for detecting where the cockpit is through raw binary data.

		int32 n_child = 0;
		int32 n_parent = 0;
		int32 n_structure = 0;
		int32 n_collision = 0;
		int32 n_wheel = 0;

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
		TArray<FWheelSetup> WheelSetups;

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
			FString FoundPartName = GetPartNameFromLocation(Pair.Key, LoadedData.NonModifiablePartData, PartTransform);
			RoundStruct(PartTransform, 50);

			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(o, 1000.0f, FColor::Cyan, FString(":::") + FoundPartName);
			TSubclassOf<UPart> *Part = GI->NameForStaticPart.Find(FoundPartName);
			UStaticMesh *ParentMeshGeo = Part->GetDefaultObject()->PartSettings.StaticMesh;
			USkeletalMesh *SkeleMeshGeo = Part->GetDefaultObject()->PartSettings.TheSkeletalMesh;
			FBox Extent = ParentMeshGeo->GetBoundingBox();
			RoundStruct(Extent, 50);
			
			UBoxComponent *ParentBox = (BoxCollisions.FindRef(Pair.Key));
			UMeshComponent *ParentMesh = (Meshes.FindRef(Pair.Key));
			FString ParentName;
			bool WasParentNull = false;

			if (ParentBox == nullptr) {
				ParentBox = NewObject<UBoxComponent>(NewVehicle, FName(*(FString("parentBox_") + FString::FromInt(n_parent))));
				if (SkeleMeshGeo != nullptr) {
					ParentMesh = Cast<UMeshComponent>(NewObject<USkeletalMeshComponent>(NewVehicle, FName(*(FString("parentSkeleMesh_") + FString::FromInt(n_parent)))));
				}
				else {
					UInstancedStaticMeshComponent *NewM = NewObject<UInstancedStaticMeshComponent>(NewVehicle, FName(*(FString("parentStaticMesh_") + FString::FromInt(n_parent))));
					ParentMesh = Cast<UMeshComponent>(NewM);
				}
				ParentMesh->SetupAttachment(ParentBox);
				ParentMesh->AddRelativeLocation(-(Extent.GetCenter()));
				ParentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				CreatedMeshes.Add(ParentMesh);

				if (Part->GetDefaultObject()->PartSettings.bUsesPhysics) {
					FormatBoxCollision(*ParentBox, true, NewVehicle);
				}
				CreatedBoxes.Add(ParentBox);
				BoxCollisions.Add(Pair.Key, ParentBox);
				Meshes.Add(Pair.Key, ParentMesh);
				SimulatedMovables.Add(ParentBox);

				WasParentNull = true;
				//CreateCollision(n_collision, MeshGeo, Parent, PartTransform.GetRotation().RotateVector(Extent.GetCenter()), TOptional<FRotator>());
				n_collision++;
			}
			if (ParentBox != nullptr) {
				ParentName = ParentBox->GetName();
				if (Pair.Key == CockpitLocation) {
					ParentBox->SetupAttachment(NewVehicle->BoxComp);
					CockpitBox = ParentBox;
					NewVehicle->MovementComp->UpdatedPrimitive = ParentBox;
					NewVehicle->CockpitBox = ParentBox;
					NewVehicle->SpringArm->SetupAttachment(CockpitBox);
					NewVehicle->SpringArm->SetRelativeLocation(FVector());

				}

				if (SkeleMeshGeo != nullptr) {
					USkeletalMeshComponent *C = Cast<USkeletalMeshComponent>(ParentMesh);
					if (C != nullptr) {
						C->SetSkeletalMesh(SkeleMeshGeo);

						if (WasParentNull) {
							if (Part->GetDefaultObject()->PartSettings.Category == ESubCategory::Wheeled) {
								/*FWheelSetup NewSetup = FWheelSetup();
								NewSetup.BoneName = *(FString("Wheel_") + FString::FromInt(n_wheel));
								NewSetup.AdditionalOffset = PartTransform.GetLocation();
								NewSetup.WheelClass = WheelData;
								WheelSetups.Add(NewSetup);
								n_wheel++;*/

								/*FWheelSetup NewSetup = FWheelSetup();
								FVector WheelSocketLoc = ParentMesh->GetSocketLocation(FName(("WheelCentre")));
								WheelSocketLoc = PartTransform.GetRotation().RotateVector(WheelSocketLoc);
								DrawDebugPoint(GetWorld(), WheelSocketLoc, 20, FColor::Orange, false, 1000, 100);
								DebugMessage(WheelSocketLoc.ToString());

								NewSetup.BoneName = *(FString("Wheel_") + FString::FromInt(n_wheel));
								NewSetup.AdditionalOffset = PartTransform.GetLocation() - CockpitLocation - ParentMeshGeo->GetBoundingBox().GetCenter() + WheelSocketLoc;
								NewSetup.WheelClass = WheelData;
								WheelSetups.Add(NewSetup);
								n_wheel++;*/
							}
						}
					}
				}
				else if (ParentMeshGeo != nullptr) {
					UInstancedStaticMeshComponent *C = Cast<UInstancedStaticMeshComponent>(ParentMesh);
					if (C != nullptr) {
						C->SetStaticMesh(ParentMeshGeo);
						if (WasParentNull) {
							C->AddInstance(FTransform(FVector(0, 0, 0)));
						}
					}
				}
				ParentBox->RegisterComponent();
				ParentMesh->RegisterComponent();

				ParentBox->SetWorldTransform(PartTransform);
				ParentBox->AddWorldOffset(PartTransform.GetRotation().RotateVector(Extent.GetCenter()));
				ParentBox->SetBoxExtent(Extent.GetExtent());

				if (InstantiatedMovableParts.Find(Pair.Key) == INDEX_NONE) {
					InstantiatedMovableParts.Add(Pair.Key);
					CreateMainStructure(NewVehicle, LoadedData, Pair.Key, MovableParts, GI, n_structure, ParentBox, n_collision);
				//	DrawDebugPoint(GetWorld(), Pair.Key, 20, FColor::Purple, false, 1000, 72);
				}
				//CreateCollision(n_collision, MeshGeo, Parent, PartTransform);
				//Comp->SetMassOverrideInKg(NAME_None, 10, true);
			}

			//---Make Children---//
			for (FVector ChildLoc : Pair.Value) {
				UBoxComponent *ChildBox = (BoxCollisions.FindRef(ChildLoc));
				UMeshComponent *ChildMesh = (Meshes.FindRef(ChildLoc));
				bool WasChildNull = false;

				FTransform ChildPartTransform;
				FString FoundPartName = GetPartNameFromLocation(ChildLoc, LoadedData.NonModifiablePartData, ChildPartTransform);
				FString ChildName;
				RoundStruct(ChildPartTransform, 50);

				TSubclassOf<UPart> *Part = GI->NameForStaticPart.Find(FoundPartName);
				if (Part == nullptr) {
					DebugMessage(FString("Part is null!"));
					return;
				}
				else if (Part->GetDefaultObject() == nullptr) {
					DebugMessage(FString("Default object is null!"));
					return;
				}
				UStaticMesh *MeshGeo = Part->GetDefaultObject()->PartSettings.StaticMesh;
				USkeletalMesh *SkeleMeshGeo = Part->GetDefaultObject()->PartSettings.TheSkeletalMesh;

				if (MeshGeo == nullptr && SkeleMeshGeo == nullptr) {
					DebugMessage(Part->GetDefaultObject()->GetName() + FString(" has no mesh!"));
				}

				if (ChildBox == nullptr) {
					ChildBox = NewObject<UBoxComponent>(NewVehicle, FName(*(FString("childBox_") + FString::FromInt(n_child))));
					
					FBox Extent;

					if (SkeleMeshGeo != nullptr) {
						USkeletalMeshComponent *NewSkele = NewObject<USkeletalMeshComponent>(NewVehicle, FName(*(FString("childSkeletalMesh_") + FString::FromInt(n_child))));
						ChildMesh = Cast<UMeshComponent>(NewSkele);
						Extent = SkeleMeshGeo->GetBounds().GetBox();

						if (Part->GetDefaultObject()->PartSettings.AnimInstance != nullptr) {
							//NewSkele->AnimScriptInstance = Part->GetDefaultObject()->PartSettings.AnimInstance.GetDefaultObject();
							NewSkele->SetAnimInstanceClass(Part->GetDefaultObject()->PartSettings.AnimInstance.Get());
						}
					}
					else if(MeshGeo!=nullptr) {
						ChildMesh = Cast<UMeshComponent>(NewObject<UInstancedStaticMeshComponent>(NewVehicle, FName(*(FString("childStaticMesh_") + FString::FromInt(n_child)))));
						Extent = MeshGeo->GetBoundingBox();
					}

					ChildMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					ChildMesh->SetupAttachment(ChildBox);
					ChildMesh->RegisterComponent();
					CreatedMeshes.Add(ChildMesh);

					if (SkeleMeshGeo != nullptr) {
						USkeletalMeshComponent *C = Cast<USkeletalMeshComponent>(ChildMesh);
						if (C != nullptr) {
							C->SetSkeletalMesh(SkeleMeshGeo);
						}
					}
					else if (MeshGeo != nullptr) {
						UInstancedStaticMeshComponent *C = Cast<UInstancedStaticMeshComponent>(ChildMesh);
						if (C != nullptr) {
							C->SetStaticMesh(MeshGeo);
							C->AddInstance(FTransform(FVector(0, 0, 0)));

						}
					}

					RoundStruct(Extent, 50);
					ChildBox->AddRelativeLocation(-(Extent.GetCenter()));
					ChildBox->SetBoxExtent(Extent.GetExtent());

					if (Part->GetDefaultObject()->PartSettings.bUsesPhysics) {
						FormatBoxCollision(*ChildBox, true, NewVehicle);
					}

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
						
						/*UInstancedStaticMeshComponent *C = Cast<UInstancedStaticMeshComponent>(ChildMesh);
						if (C != nullptr) {
							C->AddInstance(FTransform(FVector(0, 0, 0)));
						}*/
						if (Part->GetDefaultObject()->PartSettings.Category == ESubCategory::Wheeled) {
							USkeletalMeshComponent *SkeletalChildMesh = Cast<USkeletalMeshComponent>(ChildMesh);
							DebugMessage("MAKING WHEEL");
							FWheelSetup NewSetup = FWheelSetup();
							FVector Correction = SkeletalChildMesh->GetSocketLocation(FName());
							FVector WheelSocketLoc = SkeletalChildMesh->GetSocketLocation(FName(*FString("WheelCentre"))) - Correction;
							WheelSocketLoc = ChildPartTransform.GetRotation().RotateVector(WheelSocketLoc);
							DrawDebugPoint(GetWorld(), WheelSocketLoc, 20, FColor::Orange, false, 1000, 100);
							DebugMessage(Correction.ToString());
							//DebugMessage(CS->SkeletalMesh->GetBounds().GetBox().GetCenter().ToString());

							NewSetup.BoneName = *(FString("Wheel_") + FString::FromInt(n_wheel));
							NewSetup.AdditionalOffset = ChildPartTransform.GetLocation() - CockpitLocation - ParentMeshGeo->GetBoundingBox().GetCenter() + WheelSocketLoc;
							NewSetup.WheelClass = WheelData;
							WheelSetups.Add(NewSetup);
							n_wheel++;
						}
					}



					ChildBox->SetWorldTransform(ChildPartTransform);
					ChildBox->AttachToComponent(ParentBox, FAttachmentTransformRules::KeepWorldTransform);
					//ChildBox->AddWorldOffset(PartTransform.GetRotation().RotateVector(Extent.GetCenter()));

					if (InstantiatedMovableParts.Find(ChildLoc) == INDEX_NONE) {
						InstantiatedMovableParts.Add(ChildLoc);
						CreateMainStructure(NewVehicle, LoadedData, ChildLoc, MovableParts, GI, n_structure, ChildBox, n_collision);
					}
				}

				//Create the constraints
				//Check if the part should actually be movable
				if (Part->GetDefaultObject()->PartSettings.bUsesPhysics) {
					UPhysicsConstraintComponent *PhysCont = NewObject<UPhysicsConstraintComponent>(NewVehicle, FName(*(FString("physConstraint_") + FString::FromInt(n_child))));
					PhysCont->SetupAttachment(ChildBox);
					PhysCont->ComponentName1.ComponentName = FName(*ChildName);
					PhysCont->ComponentName2.ComponentName = FName(*ParentName);
					PhysCont->SetDisableCollision(true);
					PhysCont->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
					PhysCont->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);
					PhysCont->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0);


					FRotator PR = ChildPartTransform.GetRotation().Rotator();

					FVector UpVec = ChildPartTransform.GetRotation().GetUpVector();
					if (UpVec == FVector::UpVector || UpVec == -FVector::UpVector) {
						//PhysCont->SetRelativeRotation(FRotator(0, 90, 0));
						PhysCont->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 0);
						PhysCont->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
						PhysCont->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
						HorConstraints.Add(PhysCont);
					}
					else {
						PhysCont->SetRelativeRotation(FRotator(0, 0, 90));
						PhysCont->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0);
						PhysCont->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Free, 0);
						PhysCont->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
						VerConstraints.Add(PhysCont);
					}
					PhysCont->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
					PhysCont->SetAngularVelocityDrive(true, true);
					PhysCont->SetAngularOrientationDrive(true, true);
					PhysCont->SetAngularDriveParams(50000.0f, 20000.0f, 0.0f);
					PhysCont->SetWorldLocation(ChildPartTransform.GetLocation());
					PhysCont->ConstraintInstance.EnableParentDominates();
					PhysCont->RegisterComponent();
				}
				n_child++;
			}
		}

		FormatBoxCollision(*CockpitBox, true, NewVehicle);
		NewVehicle->MovementComp->WheelSetups = WheelSetups;
		NewVehicle->MovementComp->CreateVehicle();
		NewVehicle->MovementComp->RecreatePhysicsState();
	}
	NewVehicle->SetActorLocation(FVector(0, 0, 2000));
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

void AVehicleConstructor::FormatBoxCollision(UBoxComponent &Box, bool bSimulatePhysics, ASimulatedVehicle *NewVehicle) {
	Box.SetSimulatePhysics(bSimulatePhysics);
	Box.SetVisibility(true);
	Box.bHiddenInGame = false;
	Box.SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);
	Box.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box.SetCollisionProfileName("BlockAll");

	//Box.IgnoreComponentWhenMoving()
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

	if (CockpitBox != nullptr) {
		APlayerController *C = GetGameInstance()->GetFirstLocalPlayerController();
		if (C == nullptr) { return; }
		FVector WorldLoc;
		FVector Dir;
		C->DeprojectMousePositionToWorld(WorldLoc, Dir);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, WorldLoc, WorldLoc + Dir * 10000, ECollisionChannel::ECC_Visibility);
		FRotator InvertedRot = CockpitBox->GetComponentRotation().GetInverse();
		if (!Hit.bBlockingHit) { Hit.ImpactPoint = Hit.TraceEnd; }
		FVector ImpactLoc = Hit.ImpactPoint;

		for (UPhysicsConstraintComponent *HCont : HorConstraints) {
			if (HCont != nullptr) {
				FVector ContLoc = HCont->GetComponentLocation();

				FRotator T = UKismetMathLibrary::MakeRotFromX(ImpactLoc - ContLoc);
				FRotator FinalRot = T - CockpitBox->GetComponentRotation();
				DrawDebugLine(GetWorld(), CockpitBox->GetComponentLocation(), HCont->GetComponentLocation(), FColor::Blue, false, 0.1, 100, 10);
				HCont->SetAngularOrientationTarget(FRotator(0, FinalRot.Yaw, 0));
			}
		}
		for (UPhysicsConstraintComponent *VCont : VerConstraints) {
			if (VCont != nullptr) {
				FVector ContLoc = VCont->GetComponentLocation();

				FRotator T = UKismetMathLibrary::MakeRotFromX(ImpactLoc - ContLoc);
				FRotator FinalRot = T - CockpitBox->GetComponentRotation();
				VCont->SetAngularOrientationTarget(FRotator(FinalRot.Pitch, 0, 0));
			}
		}
	}

}

UInstancedStaticMeshComponent *AVehicleConstructor::CreateMesh(TSubclassOf<UPart> SelectedPart) {
	UStaticMesh *MeshToCreate = SelectedPart.GetDefaultObject()->PartSettings.StaticMesh;
	if (MeshToCreate == nullptr) { return nullptr; }
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
	UMaterialInterface * Material = SelectedPart.GetDefaultObject()->PartSettings.StaticMesh->GetMaterial(0);

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

	for (UPart *PartToDelete : MenuRef->ExistingModifiableParts) {
		if (PartToDelete != nullptr) {
			if (PartToDelete->SkeletalMeshRef != nullptr) { PartToDelete->SkeletalMeshRef->DestroyComponent(); }
			if (PartToDelete->StaticMeshRef != nullptr) { PartToDelete->StaticMeshRef->DestroyComponent(); }
			PartToDelete->DestroyComponent(true);
		}
		MenuRef->ExistingModifiableParts.Empty();
	}
	for (USkeletalPartMesh *PartToDelete : MenuRef->ExistingSkeletals) {
		PartToDelete->DestroyComponent(true);
		MenuRef->ExistingModifiableParts.Empty();
	}
	for (UStaticPartMesh *PartToDelete : MenuRef->ExistingStatics) {
		PartToDelete->DestroyComponent(true);
		MenuRef->ExistingModifiableParts.Empty();
	}
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

void AVehicleConstructor::CreateMainStructure(ASimulatedVehicle *NewVehicle, FVehicleData &LoadedData, FVector &ChildLoc, FOccluderVertexArray &MovableParts, UPlatformerGameInstance * GI, int32 &n_structure, UBoxComponent * ParentBoxPtr, int32 &n_collision)
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
		//TArray<FVector> PartsToScanTemp = PartsToScan;
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
			FString FoundPartName = GetPartNameFromLocation(ScanningPart, LoadedData.NonModifiablePartData, PartTransform);
			RoundStruct(PartTransform, 50);

			TSubclassOf<UPart> *Part = GI->NameForStaticPart.Find(FoundPartName);
			//if (Part == nullptr) { DebugMessage(FString("Couldnt find structural part: ") + FString(FoundPartName) + FString(" with transform: ") + ScanningPart.ToString()); return; }
			if (Part != nullptr) {

				if (Part->GetDefaultObject() == nullptr) {
					DebugMessage(FString("Default object is null!"));
					return;
				}
				else if (Part->GetDefaultObject()->PartSettings.StaticMesh == nullptr) {
					DebugMessage(FString("Static Mesh is null!"));
					return;

				}
				UStaticMesh *MeshGeo = Part->GetDefaultObject()->PartSettings.StaticMesh;

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
					ExistingMesh = NewObject<UInstancedStaticMeshComponent>(NewVehicle, FName(*(FString("structure_") + FString::FromInt(n_structure))));
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

				UBoxComponent *BoxCol = NewObject<UBoxComponent>(NewVehicle, FName(*(FString("structureCollision_") + FString::FromInt(n_collision))));
				BoxCol->SetBoxExtent(Extent.GetExtent());
				BoxCol->AttachToComponent(ParentBoxPtr, FAttachmentTransformRules::KeepWorldTransform);
				BoxCol->SetWorldTransform(PartTransform);
				BoxCol->AddWorldOffset(PartTransform.GetRotation().RotateVector(Extent.GetCenter()));
				BoxCol->SetMassOverrideInKg(NAME_None, 100, true);
				CreatedBoxes.Add(BoxCol);
				FormatBoxCollision(*BoxCol, false, NewVehicle);

				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(NewVehicle);
				BoxCol->MoveIgnoreActors = ActorsToIgnore;
				BoxCol->RegisterComponent();
				//CreateCollision(n_collision, MeshGeo, Child, PartTransform.GetLocation() + PartTransform.GetRotation().RotateVector(Extent.GetCenter()), PartTransform.GetRotation().Rotator());
				n_collision++;
				// Buffer the parts around it


			}
		}
	}
}
