// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneManagement.h"
#include "VehicleConstructor.generated.h"

USTRUCT()
struct FVehicleData {
	GENERATED_USTRUCT_BODY();

	TMap<FString, TArray<FTransform>> PartData;
	TMap<FVector, TArray<FVector>> WeldedParts;
	TMap<FVector, TArray<FVector>> ParentChildHierachy;
	TMap<FVector, FVector> MovablePartToRoot;
	TOptional<FVector> CockpitLocation;
};

UCLASS()
class S08_NETWORKINGCOURSE_API AVehicleConstructor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVehicleConstructor();

public:
	TOptional<FVector> TEST;
	TArray<class UPhysicsConstraintComponent*> HorConstraints;
	TArray<class UPhysicsConstraintComponent*> VerConstraints;

	class UBoxComponent *CockpitBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class UInstancedStaticMeshComponent*> InstancedMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<class UWidgetComponent *> SocketIndicators;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBoxComponent *BoxComp;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class USpringArmComponent *SpringArm; 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UCameraComponent *Camera;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class ULineBatchComponent *LineComponent;

	TArray<class UBoxComponent*> SimulatedMovables;
	class UMenu* MenuRef;
	TMap<class UInstancedStaticMeshComponent*, TSubclassOf<class APart>> PartToMesh;

	TArray<class UBoxComponent*> CreatedBoxes;
	TArray<class UInstancedStaticMeshComponent*> CreatedMeshes;
	int32 MessageIndex = 70;
	int order=0;
public:
	bool bSimulatingVehicle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UInstancedStaticMeshComponent *CreateMesh(TSubclassOf<class APart> SelectedPart);

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

	void FormatBoxCollision(UBoxComponent & Box, bool bSimulatePhysics);

	UFUNCTION(BlueprintNativeEvent)
		void SetGates(bool bGateState);

protected:
	void CreateMainStructure(FVehicleData & LoadedData, FVector & ChildLoc, FOccluderVertexArray & MovableParts, class UPlatformerGameInstance * GI, int32 & n_structure, UBoxComponent * ParentBoxPtr, int32 & n_collision);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
