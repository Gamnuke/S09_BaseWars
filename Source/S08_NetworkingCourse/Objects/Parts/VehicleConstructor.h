// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VehicleConstructor.generated.h"

UCLASS()
class S08_NETWORKINGCOURSE_API AVehicleConstructor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVehicleConstructor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class UInstancedStaticMeshComponent*> InstancedMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<class UWidgetComponent *> SocketIndicators;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//	class UStaticMeshComponent *PartImage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

		UInstancedStaticMeshComponent *CreateMesh(TSubclassOf<class APart> SelectedPart);

};
