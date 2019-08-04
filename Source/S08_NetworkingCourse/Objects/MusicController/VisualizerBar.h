// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisualizerBar.generated.h"

UCLASS()
class S08_NETWORKINGCOURSE_API AVisualizerBar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVisualizerBar();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UStaticMeshComponent *BarMesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USceneComponent* Root;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
