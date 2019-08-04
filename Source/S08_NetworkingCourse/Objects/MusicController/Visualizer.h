// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Visualizer.generated.h"

UCLASS()
class S08_NETWORKINGCOURSE_API AVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class AVisualizerBar*> Bars;

public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		int32 BarCount = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float BarThiccness = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float BarSpacing = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class AVisualizerBar> BarClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USoundWave *CurrentMusic;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StartTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool AffectsFOV;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Reverse;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UsesBars = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float BarHeightMultiplier;
};
