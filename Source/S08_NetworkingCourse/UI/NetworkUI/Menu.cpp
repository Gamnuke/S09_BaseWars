// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "Objects/Parts/VehicleConstructor.h"
#include "Objects/Parts/Part.h"
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
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Objects/Parts/SkeletalPartMesh.h"
#include "Objects/Parts/StaticPartMesh.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Objects/Parts/Movement/Wheel.h"
#include "Objects/Parts/Weaponry/TurretBasePart.h"
#include "UI/BuildModeUI/PartSettingUI/PartSettingMenu.h"
#include "UI/BuildModeUI/PartSettingUI/ComplexFloatSetting.h"

void UMenu::BlueprintTick(FGeometry Geometry, float DeltaTime)
{
	FindVehicleConstructor();

	FVector2D selectionViewportLoc;
	FVector2D selectionPixelPos;

	if (ToolToSelect != nullptr) {
		USlateBlueprintLibrary::LocalToViewport(GetWorld(), ToolToSelect->GetCachedGeometry(), FVector2D(0, 0), selectionPixelPos, selectionViewportLoc);
		ToolSelection->SetRenderTranslation(
			FMath::Vector2DInterpTo(
				ToolSelection->RenderTransform.Translation,
				selectionViewportLoc,
				DeltaTime,
				30
			)
		);
	}

	float throbVal = 1 + FMath::Sin(GetWorld()->GetTimeSeconds() * 5) * 0.05;
	ToolSelection->SetRenderScale(FVector2D(throbVal,throbVal));

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

	if (CurrentTool == ECurrentTool::PlaceTool && BuilderPawn != nullptr && BuilderPawn->StaticPartImage != nullptr) {
		DrawDebugDirectionalArrow(GetWorld(), BuilderPawn->StaticPartImage->GetComponentLocation(), BuilderPawn->StaticPartImage->GetComponentLocation() + BuilderPawn->StaticPartImage->GetForwardVector()* 100, 500, FColor::Red, false, DeltaTime * 2, 16, SkeletonVisibilityValue * 10);

	}

	if (VehicleConstructor != nullptr) {
		FVector Location;
		FVector Dir;
		GetOwningPlayer()->DeprojectMousePositionToWorld(Location, Dir);

		FCollisionQueryParams Params = FCollisionQueryParams();
		Params.bTraceComplex = true;
		Params.AddIgnoredComponent(BuilderPawn->StaticPartImage);
		Params.AddIgnoredComponent(BuilderPawn->SkeletalPartImage);

		if (GetWorld()->LineTraceSingleByChannel(OutHit, Location, Location + Dir * 10000, ECollisionChannel::ECC_Vehicle, Params)) {
			//DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 100, 5, FColor::Red, false, 5, 2, 10);
			FVector RL; // Rounded Location

			RL = OutHit.ImpactPoint + OutHit.ImpactNormal * 50;
			RoundVector(RL);

			DrawDebugPoint(GetWorld(), RL, 10, FColor::Red, false, DeltaTime, 10);

			if (PreviousMouseLocation != RL) {
				PreviousMouseLocation = RL;
				ApplyNewTool();
			}

		}
		if (BuilderPawn == nullptr) { return; }
		USceneComponent *PartHolder = BuilderPawn->PartImageHolder;

		FQuat CR = PartHolder->GetComponentRotation().Quaternion();
		FQuat OR = IntendedPartRotation.Quaternion();

		FRotator SmoothedRot = FMath::QInterpTo(CR, OR, DeltaTime, 40).Rotator();
		FVector SmoothVec = FMath::VInterpTo(PartHolder->GetComponentLocation(), IntendedPartLocation, DeltaTime, 40);
		//FTransform(FMath::RInterpTo())
		PartHolder->SetWorldLocation(SmoothVec);
		PartHolder->SetWorldRotation(SmoothedRot);
		BuilderPawn->StaticPartImage->SetRelativeTransform(FTransform());
		BuilderPawn->SkeletalPartImage->SetRelativeTransform(FTransform());
		//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, IntendedPartLocation.ToString());

	}

}
void UMenu::ApplyNewTool()
{
	switch (CurrentTool)
	{
	case PlaceTool:
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, FString("Place Tool"));
		if (SelectedPart != nullptr) {
			SetPartPlacementImage();
		}
		ToolToSelect = PlaceToolW;
		/*if (BuilderPawn != nullptr && BuilderPawn->PartImageHolder != nullptr) {
		BuilderPawn->StaticPartImage->SetVisibility(true);
		BuilderPawn->SkeletalPartImage->SetVisibility(true);
		}*/

		break;
	case DeleteTool:
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, FString("Delete Tool"));
		BuilderPawn->StaticPartImage->SetVisibility(false);
		BuilderPawn->SkeletalPartImage->SetVisibility(false);

		HighlightPart();
		ToolToSelect = DeleteToolW;

		break;
	case ConfigureTool:
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, FString("Configure Tool"));
		BuilderPawn->StaticPartImage->SetVisibility(false);
		BuilderPawn->SkeletalPartImage->SetVisibility(false);

		HighlightPart();
		ToolToSelect = ConfigureToolW;

		break;
	case PaintTool:
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(17, 1, FColor::Black, FString("Paint Tool"));
		BuilderPawn->StaticPartImage->SetVisibility(false);
		BuilderPawn->SkeletalPartImage->SetVisibility(false);

		HighlightPart();

		break;
	default:
		break;
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
		Data.NonModifiablePartData = NonModifiablePartData;
		Data.WeldedParts = WeldedParts;
		Data.CockpitLocation = CockpitLocation;

		for (UPart *PartToSave : ExistingModifiableParts) {
			FString PartName = PartToSave->GetClass()->GetName();
			PartName.RemoveFromStart("Default__");
			PartName.RemoveFromEnd("_C");
			VehicleConstructor->DebugMessage(PartToSave->PartSettings.PartLocation.ToString());
			TArray<FPartStats> *ExistingArray = Data.ModifiablePartStats.Find(PartName);
			if (ExistingArray == nullptr) {
				TArray<FPartStats> NewStats;
				NewStats.Add(PartToSave->PartSettings);
				Data.ModifiablePartStats.Add(PartName, NewStats);
			}
			else {
				TArray<FPartStats> NewStats = *ExistingArray;
				NewStats.Add(PartToSave->PartSettings);
				Data.ModifiablePartStats.Remove(PartName);
				Data.ModifiablePartStats.Add(PartName, NewStats);
			}
		}
		//Data.ModifiablePartStats = ModifiablePartStats;

		
		FPartStats NewStatsTest = FPartStats();
		NewStatsTest.NameTest = "YEEEEEEET";
		Data.Yeet = NewStatsTest;

		FBufferArchive Ar;
		SaveGameDataToFile(LoadedVehiclePath, Ar, Data);
	}
}

void UMenu::RefreshVehicles() {
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

	if (GetGameInstance() != nullptr) {
		GI = Cast<UPlatformerGameInstance>(GetGameInstance());
		if (GI == nullptr) { return; }

		int i = 0;

		for (TPair<TSubclassOf<UPart>, ESubCategory> pair : GI->CategoryOfStaticPart)
		{
			if (pair.Value == Category) {
				FString PartName = pair.Key.GetDefaultObject()->GetName();

				PartName.RemoveFromStart("Default__");
				PartName.RemoveFromEnd("_C");
				ConstructDataObject(PartName, GI->Items.FindRef(PartName), GI, pair.Key, this);

			}
			i++;
		}
	}
}

void UMenu::SetDetails(FString ItemNameToSelect)
{
	if (GI == nullptr) { return; }
	TSubclassOf<UPart> FoundPart = GI->NameForStaticPart.FindRef(ItemNameToSelect);
	if (FoundPart != nullptr) {
		UPart *BasePart = FoundPart.GetDefaultObject();
		//DetailsBox->SetText(FText::FromString(Stats.GetText()));
		ItemNameBlock->SetText(FText::FromString(ItemNameToSelect));
		DetailsCostBox->SetText(FText::FromString(FString("[") + FString::FromInt(BasePart->PartSettings.Cost) + FString(" GB") + FString("]")));
	}
}

void UMenu::SelectItem(FString ItemToPurchase) {
	SelectedPartName = ItemToPurchase;
	bool Locked = GI->Items.FindRef(ItemToPurchase);

	TSubclassOf<UPart> FoundPart = GI->NameForStaticPart.FindRef(ItemToPurchase);
	if (FoundPart != nullptr) {
		if (Locked) {
			UPart *BasePart = FoundPart.GetDefaultObject();
			PurchaseConfirmationBox->SetVisibility(ESlateVisibility::Visible);
			ItemToPurchaseBox->SetText(FText::FromString(ItemToPurchase));
			ItemCostDisplay->SetText(FText::FromString(FString::FromInt(BasePart->PartSettings.Cost)));
		}
		else {
			// Determine the mesh to be instanciated as
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
	if (CurrentTool != ECurrentTool::PlaceTool) {
		BuilderPawn->StaticPartImage->SetVisibility(false);
		BuilderPawn->SkeletalPartImage->SetVisibility(false);
		return;
	}
	else {
		BuilderPawn->StaticPartImage->SetVisibility(true);
		BuilderPawn->SkeletalPartImage->SetVisibility(true);
	}

	//Benefits of using skeletal mesh component at buildphase:
		//Can play an animation
		//Dont need to make a static mesh for each skeletalmesh
		//Dont need to duplicate sockets

	//Disadvantages:
		//complicated

	//Determine what mesh we should place
	bool bPlacingAsSkeletal = false;
	bool bIsModifiable = false;
	bool bIsPlacingCockpit = false;

	FPartStats PlacingPartSettings = SelectedPart.GetDefaultObject()->PartSettings;
	if (PlacingPartSettings.TheSkeletalMesh != nullptr) {
		//Part image as USkeletalMeshComponent
		//Build as USkeletalMeshComponent
		bPlacingAsSkeletal = true;
		BuilderPawn->SkeletalPartImage->SetSkeletalMesh(SelectedPart.GetDefaultObject()->PartSettings.TheSkeletalMesh);
		BuilderPawn->StaticPartImage->SetVisibility(false);
		BuilderPawn->SkeletalPartImage->SetVisibility(true);

		if (PlacingPartSettings.bModifiable) { bIsModifiable = true; }

	}
	else if (PlacingPartSettings.StaticMesh != nullptr) {
		BuilderPawn->StaticPartImage->SetStaticMesh(SelectedPart.GetDefaultObject()->PartSettings.StaticMesh);
		BuilderPawn->StaticPartImage->SetVisibility(true);
		BuilderPawn->SkeletalPartImage->SetVisibility(false);

		if (PlacingPartSettings.bModifiable || PlacingPartSettings.bUsesPhysics) {
			//Put it in the ParentChildHierachy variable

			//Part image as UStaticMeshComponent
			//Build as UInstancedStaticMeshComponent
			if (PlacingPartSettings.bModifiable) { bIsModifiable = true; }
		}
		else {
			//Part image as UStaticMeshComponent
			//Build as UInstancedStaticMeshComponent
		}
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("PART HAS NO SKELETALMESH OR STATICMESH"));
	}



	if (SelectedPart.GetDefaultObject()->PartSettings.Category == ESubCategory::Cockpits) { // if we are a cockpit...
		bIsPlacingCockpit = true;
	}

	BuilderPawn->CanPlace = false;
	CanPlaceItem = false;
	PendingMovablePartToRoot.Reset();
	PendingWelds.Empty();
	LikeSockets.Empty();

	FVector SnappedLoc; // Rounded Location

	SnappedLoc = OutHit.ImpactPoint;// + OutHit.ImpactNormal * 50;
	if (!OutHit.IsValidBlockingHit()) {
		SnappedLoc = OutHit.TraceEnd;
	}
	RoundVector(SnappedLoc);


	//IntendedPartTransform.SetLocation(RL);
	FVector InitialLocation = BuilderPawn->PartImageHolder->GetComponentLocation();
	FRotator InitialRotation = BuilderPawn->PartImageHolder->GetComponentRotation();
	//BuilderPawn->PartImageHolder->SetWorldRotation(IntendedPartRotation);
	//BuilderPawn->PartImageHolder->SetWorldLocation(IntendedPartLocation);

	for (int32 i = 0; i < 5; i++)
	{
		FVector CheckCollision = SnappedLoc + OutHit.ImpactNormal * i * 100;

		RoundVector(CheckCollision);

		BuilderPawn->PartImageHolder->SetWorldLocation(CheckCollision);
		BuilderPawn->PartImageHolder->SetWorldRotation(IntendedPartRotation);
		BuilderPawn->StaticPartImage->SetRelativeTransform(FTransform());
		BuilderPawn->SkeletalPartImage->SetRelativeTransform(FTransform());

		TArray<UPrimitiveComponent*> OverlappingComps;
		if (bPlacingAsSkeletal) {
			BuilderPawn->SkeletalPartImage->GetOverlappingComponents(OverlappingComps);
		}
		else {
			BuilderPawn->StaticPartImage->GetOverlappingComponents(OverlappingComps);
		}

		if (OverlappingComps.Num() == 0) {
			//IntendedPartTransform.SetLocation(OtherSocket);
			BuilderPawn->PartImageHolder->SetWorldTransform(FTransform(InitialRotation, InitialLocation));
			IntendedPartLocation = (CheckCollision);
			CanPlaceItem = true;
			break;
		}
	}
	if (!CanPlaceItem) { 
		DebugMessage(FString("PART IS OVERLAPPING"));
		return;
	} //If the part isn't overlapping with anything then it can continue.

	//Check how many sockets the placing part is joined to
	FVector SphereCentre = IntendedPartLocation;
	float Radius = 2000.0f;

	TArray<FVector> FoundSocketLocations; //Location of all the other sockets around this part.

	TMultiMap<FVector, FVector> SocketToPartLocation; // Surrounding part socket locations : Location of the part the socket belongs to
	for (UInstancedStaticMeshComponent *MeshComp : VehicleConstructor->InstancedMeshes) {
		if (MeshComp == nullptr) { return; }
		TArray<int32> CheckOverlap = MeshComp->GetInstancesOverlappingSphere(SphereCentre, Radius);
		TArray<FName> SocketNames = MeshComp->GetAllSocketNames();

		for (int32 Index : CheckOverlap)
		{
			FTransform IndexTrans;
			MeshComp->GetInstanceTransform(Index, IndexTrans, false);

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

	//Get sockets of surrounding skeletal and static meshes
	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByChannel(OverlapResults, SphereCentre, FRotator::ZeroRotator.Quaternion(), ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(Radius));
	for (FOverlapResult Result : OverlapResults) {
		if (Result.GetComponent()!=nullptr) {
			TArray<FName> SocketNames;// = FunctionalPartCast->GetAllSocketNames();
			class UStaticPartMesh *StaticMeshCast = Cast<UStaticPartMesh>(Result.GetComponent());
			class USkeletalPartMesh *SkeletalMeshCast = Cast<USkeletalPartMesh>(Result.GetComponent());
			
			if (StaticMeshCast != nullptr) {
				SocketNames = StaticMeshCast->GetAllSocketNames();
			}
			else if (SkeletalMeshCast != nullptr) {
				SocketNames = SkeletalMeshCast->GetAllSocketNames();
			}

			FVector RoundedLoc = Result.GetComponent()->GetComponentLocation();
			RoundVector(RoundedLoc);

			for (FName SocketName : SocketNames) {
				FVector SocketLocation = StaticMeshCast != nullptr ? StaticMeshCast->GetSocketLocation(SocketName) : SkeletalMeshCast->GetSocketLocation(SocketName);
				RoundVectorForSocket(SocketLocation);
				FoundSocketLocations.Add(SocketLocation);
				SocketToPartLocation.Add(SocketLocation, RoundedLoc);
			}
		}
	}

	TArray<FVector> CurrentPartSocketLocations; //The locations of these sockets of the part we're placing.
	TOptional <FVector> FoundRoot;
	FoundRoot.Reset();
	bool IsMovable = false; 

	TArray<FName> SocketNames = bPlacingAsSkeletal ?
		BuilderPawn->SkeletalPartImage->GetAllSocketNames() :
		BuilderPawn->StaticPartImage->GetAllSocketNames();

	//Gets all the socket names for the sockets in the part that we are placing.
	BuilderPawn->PartImageHolder->SetWorldTransform(FTransform(IntendedPartRotation, IntendedPartLocation));
	for (FName SocketName : SocketNames) {
		FVector SocketLocation = bPlacingAsSkeletal ?
			BuilderPawn->SkeletalPartImage->GetSocketLocation(SocketName):
			BuilderPawn->StaticPartImage->GetSocketLocation(SocketName); 

		RoundVectorForSocket(SocketLocation);

		if (SocketName.ToString().Find(FString("Socket")) != INDEX_NONE) {
			

			//SocketLocation = IntendedPartRotation.RotateVector(SocketLocation);
			//SocketLocation += IntendedPartLocation;
			CurrentPartSocketLocations.Add(SocketLocation);
			DrawDebugPoint(GetWorld(), SocketLocation, 5, FColor::Orange, false, 0.5, 40);
		}

		if (SocketName.ToString() == FString("RootWeld")) {
			CurrentPartSocketLocations.Add(SocketLocation);
			IsMovable = true;
			FVector *Root = SocketToPartLocation.Find(SocketLocation);
			DrawDebugPoint(GetWorld(), SocketLocation, 30, FColor::Purple, false, 0.5, 40);
			if (Root != nullptr) {
				FoundRoot = *Root;
				TMap<FVector, FVector> P;
				P.Add(IntendedPartLocation, FoundRoot.GetValue());
				PendingMovablePartToRoot = P;
				//DrawDebugPoint(GetWorld(), FoundRoot.GetValue(), 30, FColor::Purple, false, 0.5, 40);
			}
		}
	}
	BuilderPawn->PartImageHolder->SetWorldTransform(FTransform(InitialRotation, InitialLocation));

	if (IsMovable && !FoundRoot.IsSet()) { //If the part has a root socket and the socket actually attaches to another part...
		DebugMessage(FString("The part is a movable and its root socket doesnt attach to anything!"));
		CanPlaceItem = false;
	}

	if (bIsPlacingCockpit) {
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

		const FVector *FoundRootOfMovable = MovablePartToRoot.Find(InitialPart); //We are checking of the part we're bonded to is a movable.
		if (FoundRootOfMovable != nullptr) {
			bool IsConflicting;
			if (*FoundRootOfMovable != IntendedPartLocation) { //If the part next to us is a movable and we are the root
				IsConflicting = true;
				PendingParent = InitialPart;
			}
			ConflictingMovables.Add(InitialPart, IsConflicting);
		}
		else if (InitialPart == CockpitLocation.GetValue()) {
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
						GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(94, 1, FColor::Green, FString("COCKPIT IS SET"));


						if (SurroundIndividualPart == CockpitLocation.GetValue()) {
							GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(95, 1, FColor::Green, FString("COCKPIT IS SET"));

							CockpitFound = true;
						}
					}
					else {
						GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(94, 1, FColor::Green, FString("COCKPIT IS NOT SET"));
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
	/*else {
		if (!bIsPlacingCockpit) {
			CanPlaceItem = false;
			DebugMessage(FString("Cockpit not found in chain"));
		}
	}*/

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
			DebugMessage(FString("USING COCKPIT AS PARENT"));
			PendingParent = CockpitLocation.GetValue();
		}
	}

	if (bIsPlacingCockpit) { // if we are a cockpit...
		if (CockpitLocation.IsSet()) {
			DebugMessage(FString("Trying to place a cockpit that already exists."));
			CanPlaceItem = false;
		}
	}
	else if (OtherPartLocations.Num() == 0) {
		if (!IsMovable && !FoundRoot.IsSet()) {
			DebugMessage(FString("You are trying to place a part that doesn't have other parts connected to it"));
			CanPlaceItem = false;
		}
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

void UMenu::CreateIndicator_Implementation(FVector PartSocket){}

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
		ConfigureItem();
		break;
	case PaintTool:
		//PaintItem();
		break;
	default:
		break;
	}
}

void UMenu::ConfigureItem() {
	if (OutHit.GetComponent() != nullptr) {
		USkeletalPartMesh *FoundSkeleMesh = Cast<USkeletalPartMesh>(OutHit.GetComponent());
		UStaticPartMesh *FoundStaticMesh = Cast<UStaticPartMesh>(OutHit.GetComponent());
		UPart *PartRef;
		if (FoundSkeleMesh != nullptr) {
			PartRef = FoundSkeleMesh->PartRef;
			/*PartSettingMenu->DetailContainer->ClearChildren();
			PartRef->MenuRef = this; 
			PartRef->OnSelected();*/
		}
		else if (FoundStaticMesh != nullptr) {
			PartRef = FoundStaticMesh->PartRef;
			/*PartSettingMenu->DetailContainer->ClearChildren();
			PartRef->MenuRef = this; 
			PartRef->OnSelected();*/
		}
		else {
			return;
		}
		if (PartRef == nullptr) { return; }
		//VehicleConstructor->DebugMessage(PartRef->GetClass()->GetFullName());
		
		/*if (Cast<UWheel>(PartRef) != nullptr) { Cast<UWheel>(PartRef)->OnSelected(); }
		else { VehicleConstructor->DebugMessage("RIP"); }
		if (Cast<class UTurretBasePart>(PartRef) != nullptr) { Cast<class UTurretBasePart>(PartRef)->OnSelected(); }*/
		for (UComplexFloatSetting *S : SettingTabs) {
			S->Destruct();
		}
		SettingTabs.Empty();
		PartSettingMenu->DetailContainer->ClearChildren(); 
		PartRef->MenuRef = this;
		PartRef->OnSelected();
	}
}


void UMenu::DeleteItem() {
	if (HighlightedMesh != nullptr) {
		FTransform PartTrans;
		HighlightedMesh->GetInstanceTransform(HighlightedItem, PartTrans, true);
		TSubclassOf<UPart> DeletingClass = VehicleConstructor->PartToMesh.FindRef(HighlightedMesh);
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


		TArray<FTransform> Transforms = NonModifiablePartData.FindRef(DeletingName);
		TArray<FTransform> NewTrans;
		for (FTransform T : Transforms) {
			if (T.GetLocation() != PartLoc) { //If the transform in the array is the same as the transform of the part we're deleting
				NewTrans.Add(T);
			}
		}
		NonModifiablePartData.Remove(DeletingName); //Remove the array of transforms in the PArtData variable for this part
		NonModifiablePartData.Add(DeletingName, NewTrans);

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
		FPartStats PlacingPartSettings = SelectedPart.GetDefaultObject()->PartSettings;

		if ((PlacingPartSettings.bUsesPhysics || PlacingPartSettings.bModifiable || PlacingPartSettings.Category == ESubCategory::Cockpits) && PendingMovablePartToRoot.IsSet()) {
			VehicleConstructor->DebugMessage("YEET");
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

		if (SelectedPart.GetDefaultObject()->PartSettings.Category == ESubCategory::Cockpits) {
			if (CockpitLocation.IsSet()) {
				return;
			}
			else {
				ParentChildHierachy.Add(IntendedPartLocation, TArray<FVector>());
				CockpitLocation = IntendedPartLocation;
				WeldedParts.Add(IntendedPartLocation, TArray<FVector>());
			}
		}
		FTransform NewTransform = FTransform(IntendedPartRotation, IntendedPartLocation);
		FString PartName = FormatPartName(SelectedPart);
		TArray<FTransform> *ExistingArrayPtr = NonModifiablePartData.Find(PartName);
		TArray<FTransform> ExistingArrayRef = NonModifiablePartData.FindRef(PartName);
		ExistingArrayRef.Add(NewTransform);

		if (ExistingArrayPtr != nullptr) {
			NonModifiablePartData.Remove(PartName);
			NonModifiablePartData.Add(PartName, ExistingArrayRef);
		}
		else {
			NonModifiablePartData.Add(PartName, ExistingArrayRef);
		}

		// If the part is either of these, make a separate UPart component, make the respective mesh and parent it to the mesh.
		if (PlacingPartSettings.bModifiable || PlacingPartSettings.bUsesPhysics || PlacingPartSettings.TheSkeletalMesh != nullptr) {
			//Make the Part scene component
			class UPart *NewPart = (NewObject<UPart>(this, *SelectedPart, MakeUniqueObjectName(VehicleConstructor, UPart::StaticClass(), FName(*FString("Part")))));
			NewPart->RegisterComponent();
			NewPart->MenuRef = this;

			FPartStats DefaultStats = PlacingPartSettings;
			FTransform NewTransform = FTransform(IntendedPartRotation, IntendedPartLocation);
			DefaultStats.PartLocation = IntendedPartLocation;
			DefaultStats.PartRotation = IntendedPartRotation;
			DefaultStats.NameTest = FString("Yeet");
			NewPart->PartSettings = DefaultStats;
			ExistingModifiableParts.Add(NewPart);

			FString PartName = FormatPartName(SelectedPart);
			TArray<FPartStats> *ExistingArrayPtr = ModifiablePartStats.Find(PartName);
			TArray<FPartStats> ExistingArrayRef = ModifiablePartStats.FindRef(PartName);
			ExistingArrayRef.Add(DefaultStats);

			if (ExistingArrayPtr != nullptr) {
				ModifiablePartStats.Remove(PartName);
				ModifiablePartStats.Add(PartName, ExistingArrayRef);
			}
			else {
				ModifiablePartStats.Add(PartName, ExistingArrayRef);
			}

			if (PlacingPartSettings.TheSkeletalMesh != nullptr) {
				//Make the part's skeletalMesh
				//Parent this to the part
				class USkeletalPartMesh *NewPartMesh = NewObject<USkeletalPartMesh>(VehicleConstructor, USkeletalPartMesh::StaticClass(), MakeUniqueObjectName(VehicleConstructor, USkeletalPartMesh::StaticClass(), FName(*FString("PartStaticMesh"))));
				NewPartMesh->SetupAttachment(VehicleConstructor->GetRootComponent());
				NewPart->SetupAttachment(NewPartMesh);
				NewPart->SkeletalMeshRef = NewPartMesh;
				NewPartMesh->SetRelativeTransform(FTransform(IntendedPartRotation, IntendedPartLocation));
				NewPartMesh->SetSkeletalMesh(PlacingPartSettings.TheSkeletalMesh);
				NewPartMesh->PartRef = NewPart;
				NewPartMesh->RegisterComponent();
				NewPartMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				NewPartMesh->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
			}
			else if (PlacingPartSettings.StaticMesh != nullptr) {
				//Make the part's StaticMesh
				//Parent this to the part
				class UStaticPartMesh *NewPartMesh = NewObject<UStaticPartMesh>(VehicleConstructor, UStaticPartMesh::StaticClass(), MakeUniqueObjectName(VehicleConstructor, UStaticPartMesh::StaticClass(), FName(*FString("PartSkeletalMesh"))));
				NewPartMesh->SetupAttachment(VehicleConstructor->GetRootComponent());
				NewPart->SetupAttachment(NewPartMesh);
				NewPart->StaticMeshRef = NewPartMesh;
				NewPartMesh->SetRelativeTransform(FTransform(IntendedPartRotation, IntendedPartLocation));
				NewPartMesh->SetStaticMesh(PlacingPartSettings.StaticMesh);
				NewPartMesh->PartRef = NewPart;
				NewPartMesh->RegisterComponent();
				NewPartMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				NewPartMesh->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
			}
		}
		else {
			//Make the part as an instance of instancedStaticMeshComponent.
			UInstancedStaticMeshComponent *InstancedComponent = VehicleConstructor->CreateMesh(SelectedPart);
			if (InstancedComponent != nullptr) {
				InstancedComponent->AddInstance(NewTransform);
				PreviousLocation = IntendedPartLocation;
			}
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

FString UMenu::FormatPartName(TSubclassOf<UPart> PartClass) {
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

void UMenu::ConstructDataObject_Implementation(const FString &ItemName, bool Locked, UPlatformerGameInstance *GI, TSubclassOf<UPart> AssignedPart, const class UMenu* ParentSelector){
}

void UMenu::LoadVehicleData(FString Path, FVehicleData &Data, bool bLoadPhysical) {
	LoadGameDataFromFile(Path, Data);

	/*int yeet = 40;
	for (TSubclassOf<UPart> *T : Data.Parts) {
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(yeet, 5, FColor::Orange, T->GetDefaultObject()->BuildTransform.ToString());
		yeet++;

	}*/

	LoadedVehiclePath = Path;
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(15, 5, FColor::Yellow, FString("Loading Vehicle..."));
	WeldedParts = Data.WeldedParts;
	MovablePartToRoot = Data.MovablePartToRoot;
	ParentChildHierachy = Data.ParentChildHierachy;
	NonModifiablePartData = Data.NonModifiablePartData;
	ModifiablePartStats = Data.ModifiablePartStats;
	CockpitLocation = Data.CockpitLocation;

	VehicleConstructor->DebugMessage(Data.Yeet.NameTest);

	if (bLoadPhysical) { //Make a bool variable to check if we want to actually load the vehicle physically.
		VehicleConstructor->RemoveMeshes();
		

		for (TPair<FString, TArray<FTransform>> Pair : NonModifiablePartData) {
			TSubclassOf<UPart> *Part = GI->NameForStaticPart.Find(Pair.Key);
			if (Part != nullptr && !Part->GetDefaultObject()->PartSettings.bModifiable) {
				UInstancedStaticMeshComponent *InstancedComponent = VehicleConstructor->CreateMesh(*Part);
				if (InstancedComponent != nullptr) {
					for (FTransform T : Pair.Value) {
						InstancedComponent->AddInstance(T);
					}
				}
			}
		}

		for (TPair<FString, TArray<FPartStats>> Pair : ModifiablePartStats) {
			TSubclassOf<UPart> *Part = GI->NameForStaticPart.Find(Pair.Key);
			if (Part != nullptr) {
				FPartStats NewSettings = Part->GetDefaultObject()->PartSettings;
				for (FPartStats Stat : Pair.Value) {
					UPart *NewPart = (NewObject<UPart>(this, *Part, MakeUniqueObjectName(VehicleConstructor, UPart::StaticClass(), FName(*FString("Part")))));
					NewPart->PartSettings = Stat;
					NewPart->MenuRef = this;
					ExistingModifiableParts.Add(NewPart);

					NewPart->RegisterComponent();
					FTransform NewTransform = FTransform(Stat.PartRotation, Stat.PartLocation);

					if (NewSettings.TheSkeletalMesh != nullptr) {
						//Make the part's skeletalMesh
						//Parent this to the part
						class USkeletalPartMesh *NewPartMesh = NewObject<USkeletalPartMesh>(VehicleConstructor, USkeletalPartMesh::StaticClass(), MakeUniqueObjectName(VehicleConstructor, USkeletalPartMesh::StaticClass(), FName(*FString("PartStaticMesh"))));
						NewPartMesh->SetupAttachment(VehicleConstructor->GetRootComponent());
						NewPart->SetupAttachment(NewPartMesh);
						NewPart->SkeletalMeshRef = NewPartMesh;
						ExistingSkeletals.Add(NewPartMesh);
						NewPartMesh->SetRelativeTransform(NewTransform);
						NewPartMesh->SetSkeletalMesh(NewSettings.TheSkeletalMesh);
						NewPartMesh->PartRef = NewPart;
						NewPartMesh->RegisterComponent();
						NewPartMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
						NewPartMesh->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
					}
					else if (NewSettings.StaticMesh != nullptr) {
						//Make the part's StaticMesh
						//Parent this to the part
						class UStaticPartMesh *NewPartMesh = NewObject<UStaticPartMesh>(VehicleConstructor, UStaticPartMesh::StaticClass(), MakeUniqueObjectName(VehicleConstructor, UStaticPartMesh::StaticClass(), FName(*FString("PartSkeletalMesh"))));
						NewPartMesh->SetupAttachment(VehicleConstructor->GetRootComponent());
						NewPart->SetupAttachment(NewPartMesh);
						NewPart->StaticMeshRef = NewPartMesh;
						ExistingStatics.Add(NewPartMesh);
						NewPartMesh->SetRelativeTransform(NewTransform);
						NewPartMesh->SetStaticMesh(NewSettings.StaticMesh);
						NewPartMesh->PartRef = NewPart;
						NewPartMesh->RegisterComponent();
						NewPartMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
						NewPartMesh->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
					}
				}
			}
			
		}
	}
}

//-----Data save and load functionality-----//
void UMenu::SaveLoadData(FArchive& Ar, FVehicleData& DataToSaveLoad)
{

	Ar << DataToSaveLoad.Yeet;
	Ar << DataToSaveLoad.WeldedParts;
	Ar << DataToSaveLoad.MovablePartToRoot;
	Ar << DataToSaveLoad.ParentChildHierachy;
	Ar << DataToSaveLoad.CockpitLocation;
	Ar << DataToSaveLoad.Parts;
	Ar << DataToSaveLoad.NonModifiablePartData;
	Ar << DataToSaveLoad.ModifiablePartStats;
		
	//TMap<TSubclassOf<UPart>, TArray<FTransform>> test;
	//Ar << test;
	////New variables to be implemented:
	
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

bool UMenu::LoadGameDataFromFile(const FString& FullFilePath, FVehicleData& DataToSaveLoad) {
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