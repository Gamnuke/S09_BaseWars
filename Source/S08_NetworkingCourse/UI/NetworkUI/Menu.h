// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/PlayerCharacter/BuilderPawn.h"
#include "Objects/Parts/VehicleConstructor.h"
#include "Menu.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum ESubCategory {
	// Components:
	Rotary, Cockpits,

	//Cosmetics:
	Lights,

	//Mobility:
	Arachnids, Hover, Pedal, Wheeled,

	// Structure:
	Armoured, Light, Medium, Shielded,

	// Weaponry:
	Barrels, Enhancers, Muzzles, WModules
};

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
	FVector PreviousLocation;

public:
	UFUNCTION(BlueprintCallable)
	void SetSubCategory(ESubCategory Category);
	UFUNCTION()
		void RefreshVehicles();

	UFUNCTION()
	void PurchaseItem();

	void LoadVehicleData(FString Path, FVehicleData Data);

	void SaveLoadData(FArchive & Ar, FVehicleData & DataToSaveLoad);

	bool SaveGameDataToFile(const FString & FullFilePath, class FBufferArchive & ToBinary, FVehicleData & DataToSaveLoad);

	bool LoadGameDataFromFile(const FString & FullFilePath, FVehicleData & DataToSaveLoad);

	UFUNCTION(Client, reliable)
	void PopulateCategories(ESubCategory Category);

	UFUNCTION(BlueprintNativeEvent)
		void GetStringFromEnum(ESubCategory Enum, FString &StringRef);

	UFUNCTION(BlueprintNativeEvent)
		void ConstructDataObject(const FString &ItemName, bool Locked, UPlatformerGameInstance *GI, TSubclassOf<APart> AssignedPart, const class UMenu *ParentSelector);

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
		void OnOverrideSave();

		void SelectItem(FString ItemToPurchase);

	void OnLeftMouseClick();

	void DeleteItem();

	void FilterFloatingParts(TArray<FVector> &FloatingParts);

	void PlaceItem();

	void FindVehicleConstructor();


public:
	ESubCategory CurrentCategory;

	UPlatformerGameInstance *GI;

	TSubclassOf<APart> SelectedPart;
	
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

	TMap<FVector, TArray<FVector>> WeldedParts; // Location of first part and and the other parts that are welded to this part.
	TMap<FVector, TArray<FVector>> ParentChildHierachy;
	TMap<FVector, FVector> MovablePartToRoot; //

	TOptional<TMap<FVector, FVector>> PendingMovablePartToRoot;
	FVector PendingParent;
	TMap<FVector, TArray<FVector>> PendingWelds;

	TArray<FVector> LikeSockets;


};
