// Fill out your copyright notice in the Description page of Project Settings.

#include "Visualizer.h"
#include "Objects/MusicController/VisualizerBar.h"
#include "Engine/World.h"
#include "SoundVisualizationStatics.h"

// Sets default values
AVisualizer::AVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVisualizer::BeginPlay()
{
	Super::BeginPlay();
	
	Bars.Empty();

	if (BarClass == nullptr || !UsesBars) { return; }
	for (int32 i = BarCount; i > -1; i--)
	{
		int32 ii = i;
		FVector NextLocation = GetActorLocation() + GetActorForwardVector() * (Reverse ? -1 : 1) * 100 * BarThiccness * ii + GetActorForwardVector() * (Reverse?-1:1) * BarSpacing * ii;
		AVisualizerBar *NewBar = GetWorld()->SpawnActor<AVisualizerBar>(BarClass, NextLocation, FRotator::ZeroRotator);
		NewBar->SetActorRelativeScale3D(FVector(BarThiccness, BarThiccness, GetActorScale3D().Z));
		Bars.Add(NewBar);
	}
}

// Called every frame
void AVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (CurrentMusic != nullptr) {
	//	TArray<float> OutSpectrum;
	//	//USoundVisualizationStatics::CalculateFrequencySpectrum(CurrentMusic, 0, GetWorld()->GetTimeSeconds() - StartTime, 0.1, BarCount, OUT OutSpectrum);

	//	int32 i = 0;
	//	for (AVisualizerBar *Bar : Bars) {
	//		if (OutSpectrum.IsValidIndex(i)) {
	//			Bar->SetActorRelativeScale3D(FVector(BarThiccness, BarThiccness, OutSpectrum[i] / 5));
	//		}
	//	}
	//}
}

