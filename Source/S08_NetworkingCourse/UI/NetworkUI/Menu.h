// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/PlayerCharacter/BuilderPawn.h"
#include "Objects/Parts/VehicleConstructor.h"
#include "BasicTypes.h"
#include "Menu.generated.h"

/**
 * 
 */

UCLASS()
class S08_NETWORKINGCOURSE_API UMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize();

public:
	UFUNCTION(BlueprintCallable)
	void BlueprintTick(FGeometry Geometry, float DeltaTime);
	void RoundVector(FVector &RL);
	void RoundVectorForSocket(FVector & RL);
	void RotateItem(FRotator DeltaRot);
	void SetPartPlacementImage();

	void SetPlacability(bool & CanPlace, bool bNewPlacability);

	void HighlightPart();

	UFUNCTION(BlueprintNativeEvent)
	void CreateIndicator(FVector PartSocket);
	//

public:
	UFUNCTION(BlueprintCallable)
	void SetSubCategory(ESubCategory Category);
	UFUNCTION()
		void RefreshVehicles();

	UFUNCTION()
	void PurchaseItem();

	void LoadVehicleData(FString Path, FVehicleData &Data, bool bLoadPhysical);

	void SaveLoadData(FArchive & Ar, FVehicleData & DataToSaveLoad);

	bool SaveGameDataToFile(const FString & FullFilePath, class FBufferArchive & ToBinary, FVehicleData & DataToSaveLoad);

	bool LoadGameDataFromFile(const FString & FullFilePath, FVehicleData & DataToSaveLoad);

	UFUNCTION(Client, reliable)
	void PopulateCategories(ESubCategory Category);

	UFUNCTION(BlueprintNativeEvent)
		void GetStringFromEnum(ESubCategory Enum, FString &StringRef);

	UFUNCTION(BlueprintNativeEvent)
		void ConstructDataObject(const FString &ItemName, bool Locked, UPlatformerGameInstance *GI, TSubclassOf<UPart> AssignedPart, const class UMenu *ParentSelector);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class USelectorTab> ItemTab;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UTileView *ItemList;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UTextBlock *ItemNameBlock;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UTextBlock *DetailsBox;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UBorder *DetailsPanel;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UTextBlock *DetailsCostBox;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UTextBlock *GBDisplay;

	// Confirmation box widgets
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UBorder *PurchaseConfirmationBox;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UWidgetSwitcher *ConfirmationBoxSwitcher;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UTextBlock *ItemToPurchaseBox;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UTextBlock *ItemCostDisplay;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UTextBlock *FinalGBBalance;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UVerticalBox *PendingPurchase;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UVerticalBox *RecoveringStatus;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UVerticalBox *RecoverFail;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UVerticalBox *RecoverSuccess;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UButton *ProceedButton;

	// Load Save buttons
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UButton *LoadVehiclesButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UButton *CreateNewVehicleButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UButton *SimulateButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UButton *OverrideSaveButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere) class UScrollBox *VehiclesBox;

	UFUNCTION(BlueprintCallable)
		void SetDetails(FString ItemNameToSelect);
	UFUNCTION()
		void SimulateVehicle();
	UFUNCTION()
		void CreateNewVehicle();
	UFUNCTION()
		void OnOverrideSave();

	void SelectItem(FString ItemToPurchase);

	void DebugMessage(FString Message);
	int32 MessageIndex = 0;
	void OnLeftMouseClick();

	void DeleteItem();

	void FilterFloatingParts(TArray<FVector> &FloatingParts);

	void PlaceItem();

	FString FormatPartName(TSubclassOf<UPart> PartClass);

	void FindVehicleConstructor();


public:
	ESubCategory CurrentCategory;

	UPlatformerGameInstance *GI;

	TSubclassOf<UPart> SelectedPart;
	
	FString SelectedPartName;
	FString LoadedVehiclePath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) class AVehicleConstructor *VehicleConstructor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) class TSubclassOf<class UUserWidget> IndicatorImage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) class TSubclassOf<class UUserWidget> VehicleTabClass;
	class UInstancedStaticMeshComponent *HighlightedMesh;
	class ABuilderPawn *BuilderPawn;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float IndicatorImageSize = 60;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SkeletonVisibilityValue = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MobileSkeletonVisibilityValue = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RootsVisibilityValue = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USoundBase *MoveImageSound;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USoundBase *PlaceSound;

	FVector PreviousMouseLocation;
	FVector IntendedPartLocation;
	TOptional<FVector> CockpitLocation;
	TArray<FVector> DisconnectedParts;

	FRotator PartRotation;
	FRotator IntendedPartRotation;

	FHitResult OutHit;

	bool CanPlaceItem = false;

	ECurrentTool CurrentTool;

	int32 HighlightedItem;

	//Variables that are to be saved and loaded//
	TMap<FVector, TArray<FVector>> WeldedParts; // Location of first part and and the other parts that are welded to this part.
	TMap<FVector, TArray<FVector>> ParentChildHierachy;
	TMap<FVector, FVector> MovablePartToRoot; //Movable part : Root part that the movable is connected to.
	TMap<FString, TArray<FTransform>> PartData; //Name of part : The transforms of each instance of the part.

	TMap<FVector, FPartStats> FunctionalPartSettings; // The part setting for each part. If the part is in the structural category or if the part at the location
	TMap<FVector, class USkeletalMeshComponent*> BuiltSkeletalMeshes;
	TMap<FVector, class UStaticMeshComponent*> BuiltStaticMeshes;
	//we're looking for isnt in this variable, then take the default settings for the part.

	//TODO TMap<UInstancedStaticMeshComponent, FPartStats> //In the future we should group blocks during building phase to allow instanced blocks to be modified, EG: Color.

	//When we select a part to place, determine if it should be instanciated as a UStaticComponent, USkeletalComponent or be an instance of a UInstancedStaticMeshComponent
	//Parts that are editable should be either a StaticComponent or SkeletalComponent

	//-----------------------//

	//Temporary variables//
	TOptional<TMap<FVector, FVector>> PendingMovablePartToRoot;
	FVector PendingParent;
	TMap<FVector, TArray<FVector>> PendingWelds;


	//-----------------------//

	TArray<FVector> LikeSockets;

	TOptional<FVector> PreviousLocation;
};
