// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "Objects/Parts/Part.h"
#include "Objects/Parts/VehicleConstructor.h"
#include "UI/SelectorTab.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/PlayerCharacter/BuilderPawn.h"

void UMenu::BlueprintTick(FGeometry Geometry, float DeltaTime)
{
	FindVehicleConstructor();

	for (TPair<FVector, TArray<FVector>> pair : WeldedParts) {
		for (FVector otherPartLoc : pair.Value) {
			DrawDebugLine(GetWorld(), pair.Key, otherPartLoc, FColor::Orange, false, DeltaTime *2, 16, 3);
			//DrawDebugLine(GetWorld(), otherPartLoc, otherPartLoc + ( otherPartLoc - pair.Key).GetSafeNormal() * 5, FColor::MakeRandomColor(), false, DeltaTime * 2, 22, 15);
		}
	}

	for (FVector DisconnectedPart : DisconnectedParts) {
		DrawDebugPoint(GetWorld(), DisconnectedPart, 30.0f, FColor::Red, false, DeltaTime, 100);
	}

	int order = 1000;
	for (TPair<FVector, TArray<FVector>> d : WeldedParts) {
		//DrawDebugPoint(GetWorld(), d.Key, 10, FColor::Red, false, DeltaTime, 10);
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(order, DeltaTime, FColor::Red, d.Key.ToString());
		order++;
		for (FVector OtherPartVec : d.Value) {
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(order, DeltaTime, FColor::Orange, FString("                    ") + OtherPartVec.ToString());
			order++;
		}
	}

	if (VehicleConstructor != nullptr) {
		FVector Location;
		FVector Dir;
		GetOwningPlayer()->DeprojectMousePositionToWorld(Location, Dir);

		FCollisionQueryParams Params = FCollisionQueryParams();
		Params.bTraceComplex = false;

		if (GetWorld()->LineTraceSingleByChannel(OutHit, Location, Location + Dir * 10000, ECollisionChannel::ECC_Vehicle, Params)) {
			//DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 100, 5, FColor::Red, false, 5, 2, 10);
			FVector RL; // Rounded Location

			RL = OutHit.ImpactPoint;
			RoundVector(RL);

			if (PreviousMouseLocation != RL) {
				PreviousMouseLocation = RL;
				switch (CurrentTool)
				{
				case PlaceTool:
					GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, FString("Place Tool"));
					if (SelectedPart != nullptr) {
						SetPartPlacementImage();
					}
					if (BuilderPawn != nullptr && BuilderPawn->PartImage != nullptr) {
						BuilderPawn->PartImage->SetVisibility(true);
					}

					break;
				case DeleteTool:
					GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, FString("Delete Tool"));
					BuilderPawn->PartImage->SetVisibility(false);

					HighlightPart();

					break;
				case ConfigureTool:
					GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, FString("Configure Tool"));
					BuilderPawn->PartImage->SetVisibility(false);

					HighlightPart();

					break;
				case PaintTool:
					GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, FString("Paint Tool"));
					BuilderPawn->PartImage->SetVisibility(false);
					
					HighlightPart();

					break;
				default:
					break;
				}
			}

		}
		UStaticMeshComponent *M = BuilderPawn->PartImage;

		FQuat CR = M->GetComponentRotation().Quaternion();
		FQuat OR = IntendedPartRotation.Quaternion();

		FRotator SmoothedRot = FMath::QInterpTo(CR, OR, DeltaTime, 1).Rotator();
		FVector SmoothVec = FMath::VInterpTo(M->GetComponentLocation(), IntendedPartLocation, DeltaTime, 40);
		//FTransform(FMath::RInterpTo())
		BuilderPawn->PartImage->SetWorldLocation(SmoothVec);
		BuilderPawn->PartImage->SetWorldRotation(SmoothedRot);

	}

}
void UMenu::RoundVector(FVector &RL)
{
	RL.X = FMath::Clamp(FMath::RoundToInt(RL.X / 100) * 100, -1500, 1500);
	RL.Y = FMath::Clamp(FMath::RoundToInt(RL.Y / 100) * 100, -1500, 1500);
	RL.Z = FMath::Clamp(FMath::RoundToInt(RL.Z / 100) * 100, -50, 1450);
}
void UMenu::RoundVectorForSocket(FVector &RL)
{
	RL.X = FMath::RoundToInt(RL.X / 50) * 50;
	RL.Y = FMath::RoundToInt(RL.Y / 50) * 50;
	RL.Z = FMath::RoundToInt(RL.Z / 50) * 50;
}
void UMenu::RotateItem(FRotator DeltaRot) {
	if (CurrentTool != ECurrentTool::PlaceTool) { return; }
	//IntendedPartTransform.SetRotation((IntendedPartTransform.GetRotation().Rotator() + DeltaRot).Quaternion());
	//FRotator R = IntendedPartTransform.GetRotation().Rotator();
	//BuilderPawn->PartImage->GetComponentRotation();
	////IntendedPartTransform.SetRotation()

	//IntendedPartTransform.SetRotation(
	//	FRotator(
	//		FMath::RoundToInt((R.Pitch + DeltaRot.Pitch) / 90) * 90,
	//		FMath::RoundToInt((R.Yaw + DeltaRot.Yaw) / 90) * 90,
	//		FMath::RoundToInt((R.Roll + DeltaRot.Roll) / 90) * 90
	//	).Quaternion()
	//);
	//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(14, 10, FColor::Cyan, FRotator(
	//	FMath::RoundToInt((R.Pitch + DeltaRot.Pitch) / 90) * 90,
	//	FMath::RoundToInt((R.Yaw + DeltaRot.Yaw) / 90) * 90,
	//	FMath::RoundToInt((R.Roll + DeltaRot.Roll) / 90) * 90
	//).ToString());

	FRotator R = IntendedPartRotation;

	IntendedPartRotation = FRotator(
		FMath::RoundToInt((R.Pitch + DeltaRot.Pitch) / 90) * 90,
		FMath::RoundToInt((R.Yaw + DeltaRot.Yaw) / 90) * 90,
		FMath::RoundToInt((R.Roll + DeltaRot.Roll) / 90) * 90
	);
	SetPartPlacementImage();
}



void UMenu::SetSubCategory(ESubCategory Category) {
	PopulateCategories(Category);
}

bool UMenu::Initialize() {
	bool Success = Super::Initialize();
	ProceedButton->OnClicked.AddDynamic(this, &UMenu::PurchaseItem);

	return true;
}



void UMenu::PopulateCategories_Implementation(ESubCategory Category) {
	ItemList->ClearListItems();
	CurrentCategory = Category;
	//if (GetGameInstance() != nullptr) {
	//	UPlatformerGameInstance *GI = Cast<UPlatformerGameInstance>(GetGameInstance());
	//	if (GI == nullptr) { return; }
	//	// Get string from enum
	//	// Get folder directory to that folder names this string
	//	// Get assets in that folder
	//	// Check if the assets are locked or unlocked.
	//	FString CategoryString;
	//	GetStringFrom6(Category, CategoryString);
	//	TArray<FString> FileDirs;
	//	IFileManager::Get().FindFilesRecursive(FileDirs, *(FPaths::ProjectContentDir() + FString("Items/")), TEXT("*.uasset"), true, false, false);
	//	FString Files;
	//	for (FString Dir : FileDirs) {
	//		//Files += FPaths::GetBaseFilename(Dir) + FString(", ");
	//		////FPaths::GetBaseFilename(Dir, false);
	//		FString SearchDir = Dir;
	//		SearchDir.RemoveFromStart(FPaths::GetPath(FPaths::ProjectContentDir()));
	//		SearchDir.RemoveFromEnd(FString(".uasset"));
	//		//UE_LOG(LogTemp, Warning, TEXT("PENDING SEARCH: %s"), *(FString("/Game") + SearchDir));
	//		ConstructorHelpers::FClassFinder<APart> Part(*(FString("/Game") + SearchDir));
	//		if (Part.Class != NULL)
	//		{
	//			//UE_LOG(LogTemp, Warning, TEXT("FOUND %s"), *FPaths::GetBaseFilename(Dir));
	//			//UE_LOG(LogTemp, Warning, TEXT("Pended path %s"), *(FString("/Game") + SearchDir));
	//			Files += FPaths::GetBaseFilename(Dir) + FString(", ");
	//			Items.Add(*Part.Class.GetDefaultObject()->GetName(), true);
	//			UE_LOG(LogTemp, Warning, TEXT("Found File: %s"), *Part.Class.GetDefaultObject()->GetName());
	//		}
	//	}
	//}

	if (GetGameInstance() != nullptr) {
		GI = Cast<UPlatformerGameInstance>(GetGameInstance());
		if (GI == nullptr) { return; }

		int i = 0;

		for (TPair<TSubclassOf<APart>, ESubCategory> pair : GI->PartsInCategory)
		{
			if (pair.Value == Category) {
				FString PartName = pair.Key.GetDefaultObject()->GetName();

				PartName.RemoveFromStart("Default__");
				PartName.RemoveFromEnd("_C");
				//Tab->NameBlock->SetText(FText::FromString(PartName));
				
				//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(i, 100, FColor::Red, *(GI->Items.Contains(PartName)?FString("Found!"):FString("Not found")));
				
				//if (GI->Items.Contains(PartName)) {
				GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(13, 10, FColor::Orange, GI->Items.Contains(PartName)?FString("Found part!!/!?!/1/1"):FString("Couldnt find part123123"));

					ConstructDataObject(PartName, GI->Items.FindRef(PartName), GI, pair.Key, this);
				//}
				//ItemList->AddItem(Tab);
			}
			i++;
		}
		/*for (TSubclassOf<APart> Part : G)) {

		}*/
	}
}

void UMenu::SetDetails(FString ItemNameToSelect)
{
	if (GI == nullptr) { return; }
	TSubclassOf<APart> FoundPart = GI->NameForPart.FindRef(ItemNameToSelect);
	if (FoundPart != nullptr) {
		FPartStats Stats = FoundPart.GetDefaultObject()->Details;
		DetailsBox->SetText(FText::FromString(Stats.GetText()));
		ItemNameBlock->SetText(FText::FromString(ItemNameToSelect));
		DetailsCostBox->SetText(FText::FromString(FString("[") + FString::FromInt(Stats.Cost) + FString(" GB") + FString("]")));
	}
}

void UMenu::SelectItem(FString ItemToPurchase) {
	SelectedPartName = ItemToPurchase;
	TSubclassOf<APart> FoundPart = GI->NameForPart.FindRef(ItemToPurchase);
	bool Locked = GI->Items.FindRef(ItemToPurchase);
	if (FoundPart != nullptr) {
		if (Locked) {
			FPartStats Stats = FoundPart.GetDefaultObject()->Details;
			PurchaseConfirmationBox->SetVisibility(ESlateVisibility::Visible);
			ItemToPurchaseBox->SetText(FText::FromString(ItemToPurchase));
			ItemCostDisplay->SetText(FText::FromString(FString::FromInt(Stats.Cost)));
		}
		else {
			SelectedPart = FoundPart;
		}
	}
}

void UMenu::SetPartPlacementImage()
{
	if (CurrentTool != ECurrentTool::PlaceTool) {
		BuilderPawn->PartImage->SetVisibility(false);
		return;
	}
	else {
		BuilderPawn->PartImage->SetVisibility(true);
	}

	FVector RL; // Rounded Location

	FRotator R = IntendedPartRotation;

	RL = OutHit.ImpactPoint;// + OutHit.ImpactNormal * 50;
	if (!OutHit.IsValidBlockingHit()) {
		RL = OutHit.TraceEnd;
	}
	RoundVector(RL);

	BuilderPawn->PartImage->SetStaticMesh(SelectedPart.GetDefaultObject()->Mesh->GetStaticMesh());


	//IntendedPartTransform.SetLocation(RL);
	FVector InitialLocation = BuilderPawn->PartImage->GetComponentLocation();

	//BuilderPawn->PartImage->SetWorldTransform(IntendedPartTransform);

	BuilderPawn->CanPlace = false;
	CanPlaceItem = false;

	for (int32 i = 0; i < 5; i++)
	{
		FVector CheckCollision = RL + OutHit.ImpactNormal * i * 100;

		RoundVector(CheckCollision);
		if (CheckCollision != PreviousLocation) {

			BuilderPawn->PartImage->SetWorldLocation(CheckCollision);
			BuilderPawn->PartImage->SetWorldRotation(R);

			TArray<UPrimitiveComponent*> Actors;
			BuilderPawn->PartImage->GetOverlappingComponents(Actors);

			if (Actors.Num() == 0) {
				//IntendedPartTransform.SetLocation(OtherSocket);
				BuilderPawn->PartImage->SetWorldLocation(InitialLocation);
				IntendedPartLocation = (CheckCollision);
				CanPlaceItem = true;
				BuilderPawn->CanPlace = true;
				break;
			}
		}
	}

	BuilderPawn->PartImage->SetWorldLocation(InitialLocation);

	if (!CanPlaceItem) { return; }

	//Check how many sockets the placing part is joined to
	FVector SphereCentre = IntendedPartLocation;
	float Radius = 200.0f;

	TArray<FVector> FoundSocketLocations;
	PendingWelds.Empty();


	// Instances in the instanced mesh component are in local space
	// Create an array of the relative location of already placed parts when detecting.
	// When the part is placed, append to a TMap<FVector, TArray<FVector>> Welds variable for
		// the location of the part placed and the location of the parts that the placed part is welded to.
		// also get the parts surrounding the part placed, get the location for those and loop through to
		// append the value so it creates a weld for the selected part.
	
	// When deleting a part, find the key in the map that matches with the part being deleted IN RELATIVE SPACE, and delete the value for that key, being the array of fvectors.
		// also get the parts surrounding the part placed, get the location for those and loop through to
		// append the value so it removes the weld that matches with the location of the part being deleted.
		// if a surrounding block ends up having 0 welds around it, make it appear red. For now, just delete it so it indicates that it works.
	
	// During combat, if a part is shot, get the relative location of that part, search for the Array of vectors that include
		// the locations of the parts that that part is bonded to and scale it to 0, remove collision, mass and functionality.
		//

	// TArray<FVector> LocationsInChain;

	//Delete a part
	// get the relative locations of the parts bonded to that part.
	// check if the location of the other part is the same as the cockpit's location.
		// if it is, break the loop, else
	// check if the location is found in LocationsInChain. - stores the location of the parts scanned
	// if it isn't, 
	// add it to LocationsInChain.
	// repeat this loop for the surrounding parts of that part.
	
	// if the cockpit's location has not been found, loop through all parts in LocationsInChain and delete them.

	//TMap<FVector, TArray<FVector>> E;
	TMap<FVector, FVector> SocketToPartLocation; // Surrounding part socket locations : Location of the part the socket belongs to
	// this is for all the sockets that are detected in that sphere
	// this is so that when a part is actually placed, we can get the socket that are to be bonded and the parts that those bonds belong to.
	// must be done before placement.
	// this is the first step into getting the CurrentPartLocationToBondedPartLocation TMap.
	
	for (UInstancedStaticMeshComponent *MeshComp : VehicleConstructor->InstancedMeshes) {
		TArray<int32> CheckOverlap = MeshComp->GetInstancesOverlappingSphere(SphereCentre, Radius);
		for (int32 Index : CheckOverlap)
		{
			FTransform IndexTrans;
			MeshComp->GetInstanceTransform(Index, IndexTrans, false);
			TArray<FName> SocketNames = MeshComp->GetAllSocketNames();

			FVector RoundedLoc = IndexTrans.GetLocation();
			RoundVector(RoundedLoc);

			for (FName SocketName : SocketNames) {
				FVector SocketLocation = MeshComp->GetSocketLocation(SocketName);
				SocketLocation = IndexTrans.GetRotation().RotateVector(SocketLocation);
				SocketLocation += IndexTrans.GetLocation();

				RoundVectorForSocket(SocketLocation);
				FoundSocketLocations.Add(SocketLocation);

				SocketToPartLocation.Add(SocketLocation, RoundedLoc);
				//DrawDebugPoint(GetWorld(), SocketLocation, 10, FColor::Cyan, false, 0.5, 10);
			}
		}
	}

	TArray<FName> SocketNames = SelectedPart.GetDefaultObject()->Mesh->GetAllSocketNames();
	TArray<FVector> CurrentPartSocketLocations;

	for (FName SocketName : SocketNames) {
		FVector SocketLocation = SelectedPart.GetDefaultObject()->Mesh->GetSocketLocation(SocketName);
		SocketLocation = IntendedPartRotation.RotateVector(SocketLocation);
		SocketLocation += IntendedPartLocation;
		CurrentPartSocketLocations.Add(SocketLocation);
		//DrawDebugPoint(GetWorld(), SocketLocation, 10, FColor::Orange, false, 0.5, 10);
	}

	for (UWidgetComponent *Comp : VehicleConstructor->SocketIndicators) {
		Comp->DestroyComponent();
	}
	VehicleConstructor->SocketIndicators.Empty();

	//TMap<FVector, TArray<FVector>> PartsToBeWelded;

	PendingWelds.Empty();
	TArray<FVector> OtherPartLocations;

	// Are we not a cockpit?
		for (FVector PartSocket : CurrentPartSocketLocations) {
			RoundVectorForSocket(PartSocket);

			for (FVector OtherSocket : FoundSocketLocations) {
				RoundVectorForSocket(OtherSocket);

				if (PartSocket == OtherSocket) {
					//PendingWelds.Add(PartSocket, OtherSocket);
					//DrawDebugPoint(GetWorld(), PartSocket, 20, FColor::Green, false, 1, 10);
					FVector FoundPart = SocketToPartLocation.FindRef(PartSocket);
					if (OtherPartLocations.Find(FoundPart) == INDEX_NONE && FoundPart != FVector()) {
						OtherPartLocations.Add(FoundPart);
					}
					CreateIndicator(PartSocket);
				}
			}
		}

		if (SelectedPart.GetDefaultObject()->Category == ESubCategory::Cockpits) { // if we are a cockpit...
			if (CockpitLocation.IsSet()) {
				BuilderPawn->CanPlace = false;
				CanPlaceItem = false;
			}
		}
		else if (OtherPartLocations.Num() == 0) {
			BuilderPawn->CanPlace = false;
			CanPlaceItem = false;
		}

	PendingWelds.Add(IntendedPartLocation, OtherPartLocations);

	


	// CHECK IF THERE ARE LIKE SOCKET LOCATIONS to ensure there is a bond and the cube is connected.

	// Get all parts in a sphere
	// for each part in that sphere, get the socket locations
	// for each socket on the part image, find the closest socket found to one in the sphere
	// set the part's location so both sockets share the same location
	// check if the part is colliding with anything else
	// repeat until there is no collision

	//else set the part image's visibility to false.
}

void UMenu::HighlightPart()
{
	if (OutHit.GetComponent() != nullptr) {
		UInstancedStaticMeshComponent *FoundMesh = Cast<UInstancedStaticMeshComponent>(OutHit.GetComponent());
		if (FoundMesh == nullptr) { return; }

		int32 I = OutHit.Item;
		FTransform ItemTrans;
		if (FoundMesh->GetInstanceTransform(I, ItemTrans, false)) {
			DrawDebugPoint(GetWorld(), ItemTrans.GetLocation(), 10, FColor::Cyan, false, 0.5, 10);
			HighlightedMesh = FoundMesh;
			HighlightedItem = I;
		}
	}
}

void UMenu::CreateIndicator_Implementation(FVector PartSocket)
{
}

void UMenu::OnLeftMouseClick() {
	switch (CurrentTool)
	{
	case PlaceTool:
		PlaceItem();
		break;
	case DeleteTool:
		DeleteItem();
		break;
	case ConfigureTool:
		//ConfigureItem();
		break;
	case PaintTool:
		//PaintItem();
		break;
	default:
		break;
	}
}

void UMenu::DeleteItem() {
	if (HighlightedMesh != nullptr) {
		FTransform PartTrans;
		HighlightedMesh->GetInstanceTransform(HighlightedItem, PartTrans, true);
		FVector PartLoc = PartTrans.GetLocation();
		RoundVector(PartLoc);

		if (CockpitLocation.IsSet() && CockpitLocation.GetValue() == PartLoc) {
			for (FVector OtherPart : WeldedParts.FindRef(CockpitLocation.GetValue())) {
				RoundVector(OtherPart);
				/*if (!WeldedParts.Contains(OtherPart)) {
					GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(18, 1, FColor::Red, FString("Couldnt find PARTOATTAPRAT"));

				}*/
				TArray<FVector> PostChangeWelds = WeldedParts.FindRef(OtherPart);
				PostChangeWelds.Remove(PartLoc);
				WeldedParts.Remove(OtherPart);
				WeldedParts.Add(OtherPart, PostChangeWelds);
				//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(16,1,FColor::Green,FString(FString("Parts removed: ") + FString::FromInt( WeldedParts.FindRef(OtherPart).Remove(PartLoc))));
			}
			WeldedParts.Remove(CockpitLocation.GetValue());
			HighlightedMesh->RemoveInstance(HighlightedItem);

			CockpitLocation.Reset();
		}

		TArray<FVector> OtherWeldedParts = WeldedParts.FindRef(PartLoc);

		for (FVector OtherPart : OtherWeldedParts) {
			RoundVector(OtherPart);
			/*if (!WeldedParts.Contains(OtherPart)) {
				GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(18, 1, FColor::Red, FString("Couldnt find PARTOATTAPRAT"));

			}*/
			TArray<FVector> PostChangeWelds = WeldedParts.FindRef(OtherPart);
			PostChangeWelds.Remove(PartLoc);
			WeldedParts.Remove(OtherPart);
			WeldedParts.Add(OtherPart, PostChangeWelds);
			//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(16,1,FColor::Green,FString(FString("Parts removed: ") + FString::FromInt( WeldedParts.FindRef(OtherPart).Remove(PartLoc))));
		}
		WeldedParts.Remove(PartLoc);
		HighlightedMesh->RemoveInstance(HighlightedItem);

		FilterFloatingParts(DisconnectedParts);

		/// DONT DELETE THIS WE NEED IT
		//if (DisconnectedParts.Num() > 0) {
		//	for (UInstancedStaticMeshComponent *MeshComp : VehicleConstructor->InstancedMeshes) {
		//		for (int32 ii = 0; ii < MeshComp->GetInstanceCount(); ii++)
		//		{
		//			FTransform MeshTransform;
		//			if (MeshComp->GetInstanceTransform(ii, MeshTransform, false)) {
		//				FVector MeshLoc = MeshTransform.GetLocation();
		//				RoundVector(MeshLoc);
		//				if (DisconnectedParts.Find(MeshLoc) != INDEX_NONE) {
		//					MeshComp->RemoveInstance(ii);
		//					// This is because removing an instance shifts the entire array backwards by 1, which messes up the deletion of other instances.
		//					ii--;
		//				}
		//			}
		//		}
		//	}
		//	//for (FVector PartToBanish : DisconnectedParts) {
		//	//	DrawDebugPoint(GetWorld(), PartToBanish, 20, FColor::MakeRandomColor(), false, 5, 10);
		//	//	WeldedParts.Remove(PartToBanish);
		//	//}
		//}

		//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(19, 1, FColor::Red, FString("Looped ") + FString::FromInt(i) + FString(" times"));

		/*if (CockpitFound) {
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(181, 1, FColor::Green, FString("COCKPIT FOUND"));

		}
		else {
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(181, 1, FColor::Red, FString("DID NOT FIND COCKPIT"));

		}*/

		

		//HighlightedMesh->RemoveInstance(HighlightedItem);

		
	}
}

void UMenu::FilterFloatingParts(TArray<FVector> &FloatingParts)
{
	if (!CockpitLocation.IsSet()) { 
		DisconnectedParts.Empty();
		for (TPair<FVector, TArray<FVector>> pair : WeldedParts) {
			DisconnectedParts.Add(pair.Key);
		}
		return;
	}
	
	DisconnectedParts.Empty();
	TArray<FVector> PartsToScan;
	TArray<FVector> ScannedParts;
	PartsToScan.Add(CockpitLocation.GetValue());

	float i = 0;
	while (PartsToScan.Num() > 0)
	{
		for (FVector ScanningPart : PartsToScan) {
			RoundVector(ScanningPart);

			TArray<FVector> SurroundingDeletedPart = WeldedParts.FindRef(ScanningPart);
			//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(19, 1, FColor::Red, FString("Surrounding parts found: ") + FString::FromInt(i) + FString(" times"));
			ScannedParts.Add(ScanningPart);

			for (FVector SurroundIndividualPart : SurroundingDeletedPart) {
				RoundVector(SurroundIndividualPart);

				if (ScannedParts.Find(SurroundIndividualPart) == INDEX_NONE) {
					PartsToScan.Add(SurroundIndividualPart);
				}

				i++;

			}

			PartsToScan.Remove(ScanningPart);

		}
	}


	for (TPair<FVector, TArray<FVector>> pair : WeldedParts) {
		if (ScannedParts.Find(pair.Key) == INDEX_NONE) {
			FloatingParts.Add(pair.Key);
		}
	}
}


void UMenu::PlaceItem()
{
	FindVehicleConstructor();

	if (SelectedPart != nullptr && CanPlaceItem) {
		FVector Location;
		FVector Dir;
		FHitResult OutHit;
		GetOwningPlayer()->DeprojectMousePositionToWorld(Location, Dir);

		for (FVector OtherPartLocation : PendingWelds.FindRef(IntendedPartLocation)) { // there is only one array
			RoundVector(OtherPartLocation);
			TArray<FVector> ChangedVec = WeldedParts.FindRef(OtherPartLocation);
			ChangedVec.Add(IntendedPartLocation);

			WeldedParts.Remove(OtherPartLocation);
			WeldedParts.Add(OtherPartLocation, ChangedVec);
		}

		WeldedParts.Append(PendingWelds);

		if (SelectedPart.GetDefaultObject()->Category == ESubCategory::Cockpits) {
			if (CockpitLocation.IsSet()) {
				return;
			}
			else {
				CockpitLocation = IntendedPartLocation;
				//WeldedParts.Add(IntendedPartLocation, TArray<FVector>());
			}
		}

		

		UInstancedStaticMeshComponent *InstancedComponent = VehicleConstructor->CreateMesh(SelectedPart);

		if (&InstancedComponent != nullptr) {
			FTransform NewTransform = FTransform(IntendedPartRotation, IntendedPartLocation);
			InstancedComponent->AddInstance(NewTransform);
			//WeldedSockets.Append(PendingWelds);
			/*for (TPair<FVector, FVector> PendingWeld : PendingWelds)
			{
				DrawDebugLine(GetWorld(), PendingWeld.Key, PendingWeld.Value, FColor::Green, false, 10000, 100, 10);
			}*/
		}
		PreviousLocation = IntendedPartLocation;

		CanPlaceItem = false;
		SetPartPlacementImage();
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 0.5, FColor::Green, FString("Part Installed Successfully"));

		BuilderPawn->PostPlaceItem();
	}

	FilterFloatingParts(DisconnectedParts);

	//RoundVector(IntendedPartTransform);

	//BuilderPawn->PartImage->SetWorldLocation(CheckCollision);

	//TArray<UPrimitiveComponent*> Actors;
	//BuilderPawn->PartImage->GetOverlappingComponents(Actors);

	//if (Actors.Num() == 0) {
	//	//IntendedPartTransform.SetLocation(OtherSocket);
	//	BuilderPawn->PartImage->SetWorldTransform(InitialTransform);
	//	IntendedPartTransform.SetLocation(CheckCollision);
	//	CanPlaceItem = true;
	//	BuilderPawn->CanPlace = true;
	//	break;
	//}
}

void UMenu::FindVehicleConstructor()
{
	if (VehicleConstructor == nullptr) {
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVehicleConstructor::StaticClass(), Actors);
		if (Actors.Num() != 0) { VehicleConstructor = Cast<AVehicleConstructor>(Actors[0]); }
	}
	if (BuilderPawn == nullptr && GetOwningPlayerPawn() != nullptr) {
		BuilderPawn = Cast<ABuilderPawn>(GetOwningPlayerPawn());
		if (BuilderPawn->MenuWidgetRef == nullptr) {
			BuilderPawn->MenuWidgetRef = this;
		}
	}
}

void UMenu::PurchaseItem() {
	if (GI == nullptr) { return; }
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 5, FColor::Green, FString("Making purchase...."));
	GI->UnlockPart(SelectedPartName, GetOwningPlayer());
}

void UMenu::GetStringFromEnum_Implementation(ESubCategory Enum, FString & StringRef){
}

void UMenu::ConstructDataObject_Implementation(const FString &ItemName, bool Locked, UPlatformerGameInstance *GI, TSubclassOf<APart> AssignedPart, const class UMenu* ParentSelector){
}
