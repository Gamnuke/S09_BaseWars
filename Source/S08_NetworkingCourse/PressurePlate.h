// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "PressurePlate.generated.h"

class AMovingObject;
UCLASS()
class S08_NETWORKINGCOURSE_API APressurePlate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressurePlate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UBoxComponent *BoxComp;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UStaticMeshComponent *Mesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USceneComponent *Scene;

	UFUNCTION(BlueprintCallable)
		void TriggerOverlap();
	UFUNCTION(BlueprintCallable)
		void TriggerOverlapEnd();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AMovingObject* PlatformToMove;
	
	
};
