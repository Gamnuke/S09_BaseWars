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
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/PlayerCharacter/BuilderPawn.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/MemoryReader.h"
#include "UI/InGameUI/StatsSystem/VehicleTab.h"

void UMenu::BlueprintTick(FGeometry Geometry, float DeltaTime)
{
	FindVehicleConstructor();

	if (SkeletonVisibilityValue != 0) {
		for (TPair<FVector, TArray<FVector>> pair : WeldedParts) {

			for (FVector otherPartLoc : pair.Value) {
				if (GetWorld() != nullptr) {
					DrawDebugDirectionalArrow(GetWorld(), pair.Key, otherPartLoc, 300, FColor::Orange, false, DeltaTime * 2, 16, SkeletonVisibilityValue * 10);
				}
				//DrawDebugLine(GetWorld(), pair.Key, pair.Key+((otherPartLoc - pair.Key) / 5), FColor::Red, false, DeltaTime * 2, 17, SkeletonVisibilityValue * 10);
				//DrawDebugPoint(GetWorld(), pair.Key, 10 * SkeletonVisibilityValue, FColor::Red, false, DeltaTime * 2, 18);
				
			}
		}
	}

	if (MobileSkeletonVisibilityValue != 0) {
		for (TPair<FVector, TArray<FVector>> pair : ParentChildHierachy) {
			for (FVector Child : pair.Value) {
				DrawDebugDirectionalArrow(GetWorld(), pair.Key, Child, 300, FColor::Green, false, DeltaTime * 2, 16, MobileSkeletonVisibilityValue * 10);
			}
		}
	}

	if (RootsVisibilityValue != 0) {
		for (TPair<FVector, FVector> pair : MovablePartToRoot)
		{
			DrawDebugLine(GetWorld(), pair.Key, pair.Value, FColor::Red, false, DeltaTime * 2, 16, RootsVisibilityValue * 10);
		}
	}

	//for (TPair<FVector, FVector> pair : MovablePartToRoot) {
	//	DrawDebugPoint(GetWorld(), pair.Key, 30.0f, FColor::Yellow, false, DeltaTime*2, 100);
	//	DrawDebugPoint(GetWorld(), pair.Value, 30.0f, FColor::Orange, false, DeltaTime * 2, 100);
	//}

	//for (FVector DisconnectedPart : DisconnectedParts) {
	//	DrawDebugPoint(GetWorld(), DisconnectedPart, 30.0f, FColor::Red, false, DeltaTime, 100);
	//}

	int order = 500;
	for (TPair<FVector, TArray<FVector>> d : ParentChildHierachy) {
		//DrawDebugPoint(GetWorld(), d.Key, 10, FColor::Red, false, DeltaTime, 10);
		order++;
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(order, DeltaTime, FColor::Red, d.Key.ToString(), false);
		for (FVector OtherPartVec : d.Value) {
			order++;
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(order, DeltaTime, FColor::Orange, FString("                    ") + OtherPartVec.ToString(), false);
		}
	}

	if (CurrentTool == ECurrentTool::PlaceTool && BuilderPawn != nullptr && BuilderPawn->PartImage != nullptr) {
		DrawDebugDirectionalArrow(GetWorld(), BuilderPawn->PartImage->GetComponentLocation(), BuilderPawn->PartImage->GetComponentLocation() + BuilderPawn->PartImage->GetForwardVector()* 100, 500, FColor::Red, false, DeltaTime * 2, 16, SkeletonVisibilityValue * 10);

	}

	if (VehicleConstructor != nullptr) {
		FVector Location;
		FVector Dir;
		GetOwningPlayer()->DeprojectMousePositionToWorld(Location, Dir);

		FCollisionQueryParams Params = FCollisionQueryParams();
		Params.bTraceComplex = true;

		if (GetWorld()->LineTraceSingleByChannel(OutHit, Location, Location + Dir * 10000, ECollisionChannel::ECC_Vehicle, Params)) {
			//DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 100, 5, FColor::Red, false, 5, 2, 10);
			FVector RL; // Rounded Location

			RL = OutHit.ImpactPoint + OutHit.ImpactNormal * 50;
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
		if (BuilderPawn == nullptr) { return; }
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
	LoadVehiclesButton->OnClicked.AddDynamic(this, &UMenu::RefreshVehicles);
	OverrideSaveButton->OnClicked.AddDynamic(this, &UMenu::OnOverrideSave);
	CreateNewVehicleButton->OnClicked.AddDynamic(this, &UMenu::CreateNewVehicle);
	SimulateButton->OnClicked.AddDynamic(this, &UMenu::SimulateVehicle);

	return true;
}

void UMenu::SimulateVehicle() {
	VehicleConstructor->SetSimulation(!VehicleConstructor->bSimulatingVehicle);
}

void UMenu::CreateNewVehicle() {
	bool Found = false;
	int32 i = 0;
	while (!Found)
	{
		FString FileSearch = FString(FPlatformProcess::UserDir()).LeftChop(10) + FString("Documents/My Games/Xordia/Vehicles/NewVehicle_") + FString::FromInt(i) + FString(".txt");
		if (!FPaths::FileExists(FileSearch)) {
			LoadedVehiclePath = FileSearch;
			FString FileContent = FString();
			FFileHelper::SaveStringToFile(FileContent, *FileSearch, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_None);
			FVehicleData Data;
			LoadVehicleData(FileSearch, Data, true);
			RefreshVehicles();
			Found = true;
			break;
		}
		i++;
	}
}

void UMenu::OnOverrideSave() {
	if (LoadedVehiclePath != FString()) {
		FVehicleData Data;
		Data.MovablePartToRoot = MovablePartToRoot;
		Data.ParentChildHierachy = ParentChildHierachy;
		Data.PartData = PartData;
		Data.WeldedParts = WeldedParts;
		Data.CockpitLocation = CockpitLocation;
		FBufferArchive Ar;
		SaveGameDataToFile(LoadedVehiclePath, Ar, Data);
	}
}

void UMenu::RefreshVehicles() {
	//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(643, 100, FColor::Red, FPaths::AutomationDir());
	//UE_LOG(LogTemp, Warning, TEXT("%s"), FPlatformProcess::UserSettingsDir());

	//FString FileSearch = FString(FPlatformProcess::UserDir()).LeftChop(10) + FString("Documents/My Games/Xordia/Vehicles/_Hello_There_") + FString(".txt");
	////if (!FPaths::FileExists(FileSearch)) {
	//FString FileContent = FString();
	//FFileHelper::SaveStringToFile(FileContent, *FileSearch, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_None);

		//FBufferArchive BufferArchive;
		//FPlayerData newPlayerData = FPlayerData();
		//newPlayerData.Items = Items;
		//SaveGameDataToFile(FileSearch, BufferArchive, newPlayerData);
	/*FindFiles
	(
		TArray< FString >& FoundFiles,
		const TCHAR* Directory,
		const TCHAR* FileExtension
	)*/
	TArray<FString> FoundFiles;
	IFileManager *F = &IFileManager::Get();
	F->FindFiles(FoundFiles, *(FString(FPlatformProcess::UserDir()).LeftChop(10) + FString("Documents/My Games/Xordia/Vehicles")), *FString(".txt"));
	VehiclesBox->ClearChildren();

	int32 l=0;
	if (VehicleTabClass != nullptr) {
		for (FString f : FoundFiles) {
			l++;
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(l, 100, FColor::Green, f);
			UVehicleTab *NewTab = CreateWidget<UVehicleTab>(this, VehicleTabClass, *FString::FromInt(l));
			NewTab->AddToViewport();
			VehiclesBox->AddChild(NewTab);
			NewTab->ParentMenu = this;
			NewTab->FilePath = FString(FPlatformProcess::UserDir()).LeftChop(10) + FString("Documents/My Games/Xordia/Vehicles/") + f;
			NewTab->VehicleName->SetText(FText::FromString(f));
		}
	}
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
				//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(13, 10, FColor::Orange, GI->Items.Contains(PartName)?FString("Found part!!/!?!/1/1"):FString("Couldnt find part123123"));

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
void UMenu::DebugMessage(FString Message) {
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(436, 1000.0f, FColor::Orange, FString::FromInt(MessageIndex - 70) + FString(" | ") + Message);
	MessageIndex++;
}

void UMenu::SetPartPlacementImage()
{
	bool IsPlacingCockpit = false;
	BuilderPawn->CanPlace = false;
	CanPlaceItem = false;
	PendingMovablePartToRoot.Reset();
	PendingWelds.Empty();
	LikeSockets.Empty();

	if (SelectedPart.GetDefaultObject()->Category == ESubCategory::Cockpits) { // if we are a cockpit...
		IsPlacingCockpit = true;
	}

	if (CurrentTool != ECurrentTool::PlaceTool) {
		BuilderPawn->PartImage->SetVisibility(false);
		return;
	}
	else {
		BuilderPawn->PartImage->SetVisibility(true);
	}

	FVector SnappedLoc; // Rounded Location
	FRotator IntendedRotation = IntendedPartRotation;

	SnappedLoc = OutHit.ImpactPoint;// + OutHit.ImpactNormal * 50;
	if (!OutHit.IsValidBlockingHit()) {
		SnappedLoc = OutHit.TraceEnd;
	}
	RoundVector(SnappedLoc);

	BuilderPawn->PartImage->SetStaticMesh(SelectedPart.GetDefaultObject()->Mesh->GetStaticMesh());

	//IntendedPartTransform.SetLocation(RL);
	FVector InitialLocation = BuilderPawn->PartImage->GetComponentLocation();

	for (int32 i = 0; i < 5; i++)
	{
		FVector CheckCollision = SnappedLoc + OutHit.ImpactNormal * i * 100;

		RoundVector(CheckCollision);

		BuilderPawn->PartImage->SetWorldLocation(CheckCollision);
		BuilderPawn->PartImage->SetWorldRotation(IntendedRotation);

		TArray<UPrimitiveComponent*> Actors;
		BuilderPawn->PartImage->GetOverlappingComponents(Actors);

		if (Actors.Num() == 0) {
			//IntendedPartTransform.SetLocation(OtherSocket);
			BuilderPawn->PartImage->SetWorldLocation(InitialLocation);
			IntendedPartLocation = (CheckCollision);
			CanPlaceItem = true;
			break;
		}
	}
	if (!CanPlaceItem) { 
		DebugMessage(FString("PART IS OVERLAPPING"));
		return; 
	} //If the part isn't overlapping with anything then it can continue.

	BuilderPawn->PartImage->SetWorldLocation(InitialLocation);


	//Check how many sockets the placing part is joined to
	FVector SphereCentre = IntendedPartLocation;
	float Radius = 2000.0f;

	TArray<FVector> FoundSocketLocations; //Location of all the other sockets around this part.

	TMultiMap<FVector, FVector> SocketToPartLocation; // Surrounding part socket locations : Location of the part the socket belongs to

	for (UInstancedStaticMeshComponent *MeshComp : VehicleConstructor->InstancedMeshes) {
		if (MeshComp == nullptr) { return; }
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
				//DrawDebugPoint(GetWorld(), SocketLocation, 10, FColor::Green, false, 0.5, 10);

				SocketToPartLocation.Add(SocketLocation, RoundedLoc);
				//DrawDebugPoint(GetWorld(), SocketLocation, 10, FColor::Cyan, false, 0.5, 10);
			}
		}
	}

	TArray<FName> SocketNames = SelectedPart.GetDefaultObject()->Mesh->GetAllSocketNames(); //Gets all the socket names for the sockets in the part that we are placing.
	TArray<FVector> CurrentPartSocketLocations; //The locations of these sockets of the part we're placing.

	TOptional <FVector> FoundRoot;
	FoundRoot.Reset();
	bool IsMovable = false;

	for (FName SocketName : SocketNames) {
		FVector SocketLocation = SelectedPart.GetDefaultObject()->Mesh->GetSocketLocation(SocketName);
		SocketLocation = IntendedPartRotation.RotateVector(SocketLocation);
		SocketLocation += IntendedPartLocation;
		RoundVectorForSocket(SocketLocation);
		CurrentPartSocketLocations.Add(SocketLocation);

		if (SocketName.ToString() == FString("Root")) {
			IsMovable = true;
			FVector *Root = SocketToPartLocation.Find(SocketLocation);
			if(Root != nullptr){
				FoundRoot = *Root;
				TMap<FVector, FVector> P;
				P.Add(IntendedPartLocation, FoundRoot.GetValue());
				PendingMovablePartToRoot = P;
				//DrawDebugPoint(GetWorld(), FoundRoot.GetValue(), 30, FColor::Red, false, 0.5, 40);
			}
		}
	}

	if (IsMovable && !FoundRoot.IsSet()) { //If the part has a root socket and the socket actually attaches to another part...
		DebugMessage(FString("The part is a movable and its root socket doesnt attach to anything!"));
		CanPlaceItem = false;
	}

	if (IsPlacingCockpit) {
		if (CockpitLocation.IsSet()) {
			DebugMessage(FString("You are placing a cockpit that already exists"));
			CanPlaceItem = false;
		}
	}
	else {
		if (!CockpitLocation.IsSet()) {
			DebugMessage(FString("You are trying to place a part while the cockpit doesn't exist."));
			CanPlaceItem = false;
		}
	}

	for (UWidgetComponent *Comp : VehicleConstructor->SocketIndicators) {
		Comp->DestroyComponent();
	}
	VehicleConstructor->SocketIndicators.Empty();

	TArray<FVector> OtherPartLocations;
	for (FVector PartSocket : CurrentPartSocketLocations) {
		RoundVectorForSocket(PartSocket);

		for (FVector OtherSocket : FoundSocketLocations) {
			RoundVectorForSocket(OtherSocket);

			if (PartSocket == OtherSocket) {
				//PendingWelds.Add(PartSocket, OtherSocket);
				//DrawDebugPoint(GetWorld(), PartSocket, 20, FColor::Green, false, 1, 10);
				LikeSockets.Add(PartSocket);
				FVector *FoundPart = SocketToPartLocation.Find(PartSocket);
				if (FoundPart != nullptr && OtherPartLocations.Find(*FoundPart) == INDEX_NONE) {
					OtherPartLocations.Add(*FoundPart);
					//DrawDebugPoint(GetWorld(), *FoundPart, 20, FColor::Magenta, false, 2, 6);
				}
				CreateIndicator(PartSocket);
			}
		}
	}

	TArray<FVector> PartsToScan;
	TArray<FVector> ScannedParts;
	TMap<FVector, bool> ConflictingMovables;
	//PartsToScan = OtherPartLocations;
	bool CockpitFound = false;

	for (FVector InitialPart : OtherPartLocations) {
		RoundVector(InitialPart);

		const FVector *FoundRootOfMovable = MovablePartToRoot.Find(InitialPart);
		if (FoundRootOfMovable != nullptr) {
			bool IsConflicting;
			if (*FoundRootOfMovable != IntendedPartLocation) { //If the part next to us is a movable and we are the root
				IsConflicting = true;
				PendingParent = InitialPart;
			}
			ConflictingMovables.Add(InitialPart, IsConflicting);
		}
		else if (InitialPart == CockpitLocation) {
			CockpitFound = true;
		}
		else { //Its a normal part, so add it to the scan buffer.
			PartsToScan.Add(InitialPart);
			//DrawDebugPoint(GetWorld(), InitialPart, 30, FColor::Cyan, false, 2, 100);
		}
	}

	while (PartsToScan.Num() != 0) {
		TArray<FVector> PartsToScanTemp = PartsToScan;
		for (FVector ScanningPart : PartsToScanTemp) {
			ScannedParts.Add(ScanningPart);
			RoundVector(ScanningPart);
			//DrawDebugPoint(GetWorld(), ScanningPart, 20, FColor::Green, false, 2, 6);


			TArray<FVector> SurroundingParts = WeldedParts.FindRef(ScanningPart);
			//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(19, 1, FColor::Red, FString("Surrounding parts found: ") + FString::FromInt(i) + FString(" times"));

			for (FVector SurroundIndividualPart : SurroundingParts) {
				RoundVector(SurroundIndividualPart);

				const FVector *FoundRootOfMovable = MovablePartToRoot.Find(SurroundIndividualPart);
				if (FoundRootOfMovable != nullptr) { //Have we found a movable part?
					if (*FoundRootOfMovable != ScanningPart) //Is the root of this movable the same as the one we're scanning?
					{
						PendingParent = SurroundIndividualPart;
						//DrawDebugPoint(GetWorld(), SurroundIndividualPart, 60, FColor::Blue, false, 2, 6);

						ConflictingMovables.Add(SurroundIndividualPart, true);
					}
				}
				else { //It isnt movable, so
					if (CockpitLocation.IsSet()) {
						if (SurroundIndividualPart == CockpitLocation.GetValue()) {
							CockpitFound = true;
						}
					}
					if (ScannedParts.Find(SurroundIndividualPart) == INDEX_NONE) { //If the part around the scanning part hasn't been scanned already...
						PartsToScan.Add(SurroundIndividualPart);
					}
				}
			}
			PartsToScan.Remove(ScanningPart);
		}
	}

	int32 NumConflicts = 0;
	if (CockpitFound) { NumConflicts++; }
	for (TPair<FVector, bool> Pair : ConflictingMovables) {
		if (Pair.Value == true) {
			DrawDebugPoint(GetWorld(), Pair.Key, 20, FColor::Purple, false, 2, 100);
			NumConflicts++;
		}
	}

	if (NumConflicts > 1) {
		DebugMessage(FString("More than 1 conflicting part found"));
		CanPlaceItem = false;
	}
	
	if (FoundRoot.IsSet()) {
		if (CockpitFound && CockpitLocation.IsSet()) {
			PendingParent = CockpitLocation.GetValue();
		}
	}

	if (IsPlacingCockpit) { // if we are a cockpit...
		if (CockpitLocation.IsSet()) {
			DebugMessage(FString("Trying to place a cockpit that already exists."));
			CanPlaceItem = false;
		}
	}
	else if (OtherPartLocations.Num() == 0) {
		DebugMessage(FString("You are trying to place a part that doesn't have other parts connected to it"));
		CanPlaceItem = false;
	}

	if (CanPlaceItem) {
		PlaySound(MoveImageSound);
	}

	BuilderPawn->CanPlace = CanPlaceItem;
	PendingWelds.Add(IntendedPartLocation, OtherPartLocations);
}

void UMenu::SetPlacability(bool &CanPlace, bool bNewPlacability) {
	CanPlace = bNewPlacability;
	BuilderPawn->CanPlace = bNewPlacability;
}

void UMenu::HighlightPart()
{
	if (OutHit.GetComponent() != nullptr) {
		UInstancedStaticMeshComponent *FoundMesh = Cast<UInstancedStaticMeshComponent>(OutHit.GetComponent());
		if (FoundMesh == nullptr) { return; }

		int32 I = OutHit.Item;
		FTransform ItemTrans;
		if (FoundMesh->GetInstanceTransform(I, ItemTrans, false)) {
			//DrawDebugPoint(GetWorld(), ItemTrans.GetLocation(), 10, FColor::Cyan, false, 0.5, 10);
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
		TSubclassOf<APart> DeletingClass = VehicleConstructor->PartToMesh.FindRef(HighlightedMesh);
		if (*DeletingClass == nullptr) { return; }
		FString DeletingName = FormatPartName(DeletingClass);
		FVector PartLoc = PartTrans.GetLocation();
		RoundVector(PartLoc);

		//If this part is a parent...
		if (ParentChildHierachy.Find(PartLoc) != nullptr && ParentChildHierachy.Find(PartLoc)->Num() != 0) { return; } //TODO Add a message saying that every other movable part on that chain must be deleted before deleting this part.

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


		TArray<FTransform> Transforms = PartData.FindRef(DeletingName);
		TArray<FTransform> NewTrans;
		for (FTransform T : Transforms) {
			if (T.GetLocation() != PartLoc) { //If the transform in the array is the same as the transform of the part we're deleting
				NewTrans.Add(T);
			}
		}
		PartData.Remove(DeletingName); //Remove the array of transforms in the PArtData variable for this part
		PartData.Add(DeletingName, NewTrans);

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

		// Sort out parent child hierachy variable, delete a parent if it has no children.
		TMap<FVector, TArray<FVector>> ParentChildHierachyTemp = ParentChildHierachy;
		for (TPair<FVector, TArray<FVector>> Pair : ParentChildHierachyTemp) {
			if (Pair.Value.Find(PartLoc) != INDEX_NONE) {
				TArray<FVector> Children = Pair.Value;
				Children.Remove(PartLoc);
				if (Children.Num() == 0) {
					ParentChildHierachy.Remove(Pair.Key);
				}
				else {
					ParentChildHierachy.Remove(Pair.Key);
					ParentChildHierachy.Add(Pair.Key, Children);
				}
			}
		}

		MovablePartToRoot.Remove(PartLoc);

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
		TArray<FVector> PartsToScanTemp = PartsToScan;
		for (FVector ScanningPart : PartsToScanTemp) {
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

		if (PendingMovablePartToRoot.IsSet()) {
			MovablePartToRoot.Append(PendingMovablePartToRoot.GetValue());
			
			TArray<FVector> *ExistingChildrenPtr = ParentChildHierachy.Find(PendingParent);
			TArray<FVector> ExistingChildrenRef = ParentChildHierachy.FindRef(PendingParent);
			ExistingChildrenRef.Add(IntendedPartLocation);

			if (ExistingChildrenPtr != nullptr) {
				ParentChildHierachy.Remove(PendingParent);
				ParentChildHierachy.Add(PendingParent, ExistingChildrenRef);
			}
			else {
				ParentChildHierachy.Add(PendingParent, ExistingChildrenRef);
			}
		}

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
		FTransform NewTransform = FTransform(IntendedPartRotation, IntendedPartLocation);
		FString PartName = FormatPartName(SelectedPart);
		TArray<FTransform> *ExistingArrayPtr = PartData.Find(PartName);
		TArray<FTransform> ExistingArrayRef = PartData.FindRef(PartName);
		ExistingArrayRef.Add(NewTransform);

		if (ExistingArrayPtr != nullptr) {
			PartData.Remove(PartName);
			PartData.Add(PartName, ExistingArrayRef);
		}
		else {
			PartData.Add(PartName, ExistingArrayRef);
		}

		UInstancedStaticMeshComponent *InstancedComponent = VehicleConstructor->CreateMesh(SelectedPart);
		if (&InstancedComponent != nullptr) {
			InstancedComponent->AddInstance(NewTransform);
			PreviousLocation = IntendedPartLocation;
		}
		PlaySound(PlaceSound);

		CanPlaceItem = false;
		
		BuilderPawn->AttachSockets = LikeSockets;
		BuilderPawn->PostPlaceItem(IntendedPartLocation);
		SetPartPlacementImage();
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 0.5, FColor::Green, FString("Part Installed Successfully"));

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

FString UMenu::FormatPartName(TSubclassOf<APart> PartClass) {
	FString PartName = PartClass.GetDefaultObject()->GetName();
	PartName.RemoveFromStart("Default__");
	PartName.RemoveFromEnd("_C");
	return PartName;
}

void UMenu::FindVehicleConstructor()
{
	if (VehicleConstructor == nullptr) {
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVehicleConstructor::StaticClass(), Actors);
		if (Actors.Num() != 0) { VehicleConstructor = Cast<AVehicleConstructor>(Actors[0]); }
		VehicleConstructor->MenuRef = this;
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

void UMenu::LoadVehicleData(FString Path, FVehicleData &Data, bool bLoadPhysical) {
	LoadGameDataFromFile(Path, Data);
	LoadedVehiclePath = Path;
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(15, 5, FColor::Yellow, FString("Loading Vehicle..."));
	WeldedParts = Data.WeldedParts;
	MovablePartToRoot = Data.MovablePartToRoot;
	ParentChildHierachy = Data.ParentChildHierachy;
	PartData = Data.PartData;
	CockpitLocation = Data.CockpitLocation;

	if (bLoadPhysical) { //Make a bool variable to check if we want to actually load the vehicle physically.
		VehicleConstructor->RemoveMeshes();

		for (TPair<FString, TArray<FTransform>> Pair : PartData) {
			TSubclassOf<APart> *Part = GI->NameForPart.Find(Pair.Key);
			if (Part != nullptr) {
				UInstancedStaticMeshComponent *InstancedComponent = VehicleConstructor->CreateMesh(*Part);
				if (&InstancedComponent != nullptr) {
					for (FTransform T : Pair.Value) {
						InstancedComponent->AddInstance(T);
					}
				}
			}
		}
	}
}

//-----Data save and load functionality-----//
void UMenu::SaveLoadData(FArchive& Ar, FVehicleData& DataToSaveLoad)
{
	Ar << DataToSaveLoad.WeldedParts;
	Ar << DataToSaveLoad.PartData;
	Ar << DataToSaveLoad.MovablePartToRoot;
	Ar << DataToSaveLoad.ParentChildHierachy;
	Ar << DataToSaveLoad.CockpitLocation;
}

bool UMenu::SaveGameDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary, FVehicleData& DataToSaveLoad)
{
	//note that the supplied FString must be the entire Filepath
	// 	if writing it out yourself in C++ make sure to use the \\
	// 	for example:

	// 	FString SavePath = "C:\\MyProject\\MySaveDir\\mysavefile.save";

	//Step 1: Variable Data -> Binary

	//following along from above examples
	SaveLoadData(ToBinary, DataToSaveLoad);
	//presumed to be global var data, 
	//could pass in the data too if you preferred

	//No Data
	if (ToBinary.Num() <= 0) return false;
	//~

	//Step 2: Binary to Hard Disk
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();

		return true;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	return false;
}

bool UMenu::LoadGameDataFromFile(
	const FString& FullFilePath,
	FVehicleData& DataToSaveLoad
) {
	//Load the data array,
	// 	you do not need to pre-initialize this array,
	//		UE4 C++ is awesome and fills it 
	//		with whatever contents of file are, 
	//		and however many bytes that is
	TArray<uint8> TheBinaryArray;
	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *FullFilePath))
	{
		return false;
		//~~
	}

	//File Load Error
	if (TheBinaryArray.Num() <= 0) return false;

	//~
	//		  Read the Data Retrieved by GFileManager
	//~

	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);
	SaveLoadData(FromBinary, DataToSaveLoad);

	//~
	//								Clean up 
	//~
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	TheBinaryArray.Empty();
	FromBinary.Close();

	return true;
}
