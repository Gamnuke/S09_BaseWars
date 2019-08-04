// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/PlayerCharacter/BuilderPawn.h"
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

	void HighlightPart();

	UFUNCTION(BlueprintNativeEvent)
	void CreateIndicator(FVector PartSocket);
	//
	FVector PreviousLocation;

public:
	UFUNCTION(BlueprintCallable)
	void SetSubCategory(ESubCategory Category);
	UFUNCTION()
	void PurchaseItem();

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


	UFUNCTION(BlueprintCallable)
		void SetDetails(FString ItemNameToSelect);

		void SelectItem(FString ItemToPurchase);

	TMap<FVector, TArray<FVector>> PendingWelds;
	TMap<FVector, TArray<FVector>> WeldedParts; // Location of first part and and the other parts that are welded to this part.

	ESubCategory CurrentCategory;

	UPlatformerGameInstance *GI;

	TSubclassOf<APart> SelectedPart;
	FString SelectedPartName;

	void OnLeftMouseClick();

	void DeleteItem();

	void FilterFloatingParts(TArray<FVector> &FloatingParts);

	void PlaceItem();

	void FindVehicleConstructor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class AVehicleConstructor *VehicleConstructor;

	FVector PreviousMouseLocation;

	FRotator PartRotation;


	FHitResult OutHit;

	bool CanPlaceItem = false;

	class ABuilderPawn *BuilderPawn;

	FVector IntendedPartLocation;
	FRotator IntendedPartRotation;

	TOptional<FVector> CockpitLocation;

	ECurrentTool CurrentTool;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class TSubclassOf<class UUserWidget> IndicatorImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float IndicatorImageSize = 60;

	class UInstancedStaticMeshComponent *HighlightedMesh;
	int32 HighlightedItem;

	TArray<FVector> DisconnectedParts;
};
