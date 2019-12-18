// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneManagement.h"
#include "WheeledVehicle.h"
#include "WheeledVehicleTest.h"
#include "BasicTypes.h"
#include "VehicleConstructor.generated.h"

USTRUCT()
struct FVehicleData {
	GENERATED_USTRUCT_BODY();

	TMap<FString, TArray<FTransform>> NonModifiablePartData;
	TMap<FVector, TArray<FVector>> WeldedParts;
	TMap<FVector, TArray<FVector>> ParentChildHierachy;
	TMap<FVector, FVector> MovablePartToRoot;
	TOptional<FVector> CockpitLocation;
	TMap < FString, TArray<FPartStats>> ModifiablePartStats;
	FPartStats Yeet;

	TArray<TSubclassOf<class UPart*>> Parts;
};

UCLASS()
class S08_NETWORKINGCOURSE_API AVehicleConstructor : public APawn
{
	GENERATED_BODY()
	
AVehicleConstructor(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class UVehicleWheel> WheelData;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class ASimulatedVehicle> VehicleClass;

		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintNativeEvent)
		void SpawnVehicle();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		ASimulatedVehicle *SpawnedVehicle;

	TOptional<FVector> TEST;
	TArray<class UPhysicsConstraintComponent*> HorConstraints;
	TArray<class UPhysicsConstraintComponent*> VerConstraints;

	class UBoxComponent *CockpitBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class UInstancedStaticMeshComponent*> InstancedMeshes;
	TMap<TSubclassOf<class UPart>*, FTransform> BuiltFunctionalParts;
	TArray<TSubclassOf<class UPart>*> BuiltFunctionalPartMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<class UWidgetComponent *> SocketIndicators;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UBoxComponent *BoxComp;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USpringArmComponent *SpringArm; 
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UCameraComponent *Camera;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class ULineBatchComponent *LineComponent;

	TArray<class UBoxComponent*> SimulatedMovables;
	class UMenu* MenuRef;
	TMap<class UInstancedStaticMeshComponent*, TSubclassOf<class UPart>> PartToMesh;

	TArray<class UBoxComponent*> CreatedBoxes;
	TArray<class UMeshComponent*> CreatedMeshes;
	int32 MessageIndex = 70;
	int order=0;

public:
	bool bSimulatingVehicle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UInstancedStaticMeshComponent *CreateMesh(TSubclassOf<class UPart> SelectedPart);

	void RemoveMeshes();

	void CheckIfMeshIsEmpty(UInstancedStaticMeshComponent * MeshToCheck);

	bool SetSimulation(bool bSimulateVehicle);

	void ReverseSimulation();

	FTransform FindTransformFromLocation(FVector Location, TArray<FTransform> Transforms);

	FString GetPartNameFromLocation(FVector LocationToSearch, TMap<FString, TArray<FTransform>> PartForName, FTransform & FoundTransform);

	void DebugMessage(FString Message);

	void BuildSimulatedVehicle();

	void RoundStruct(FBox & Extent, int32 RoundTo);

	void RoundStruct(FTransform & Transform, int32 RoundTo);

	void FormatBoxCollision(UBoxComponent & Box, bool bSimulatePhysics, class ASimulatedVehicle *NewVehicle);

	UFUNCTION(BlueprintNativeEvent)
		void SetGates(bool bGateState);

protected:
	void CreateMainStructure(class ASimulatedVehicle *NewVehicle, FVehicleData & LoadedData, FVector & ChildLoc, FOccluderVertexArray & MovableParts, class UPlatformerGameInstance * GI, int32 & n_structure, UBoxComponent * ParentBoxPtr, int32 & n_collision);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
