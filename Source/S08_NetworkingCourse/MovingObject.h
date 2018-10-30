// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingObject.generated.h"


UCLASS()
class S08_NETWORKINGCOURSE_API AMovingObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (MakeEditWidget = true))
		FVector TargetLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsTriggered = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MoveSpeed = 5;

	
	FVector CStartLocation;
	FVector CTargetLocation;

	FVector CurrentTargetLocation;

	bool Moving;
};
