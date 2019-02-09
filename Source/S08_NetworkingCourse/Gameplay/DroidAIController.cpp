// Fill out your copyright notice in the Description page of Project Settings.

#include "DroidAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Droid.h"
#include "Gameplay/MainCharacter.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

ADroidAIController::ADroidAIController() {
	UE_LOG(LogTemp, Warning, TEXT("INITIIETETSOTEset"));
	PrimaryActorTick.bCanEverTick = true;
}

void ADroidAIController::BeginPlay() {
	Super::BeginPlay();
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Green, FString("fucking end me"));
	UE_LOG(LogTemp, Warning, TEXT("BEBGIGEGIN"));
}

void ADroidAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (GetWorld()->GetTimeSeconds() > NextSwitchTime) {
		NextSwitchTime = GetWorld()->GetTimeSeconds() + 5;
		SetTargetLocation();
	}
	if (TargetCharacter != nullptr) {
		MoveToLocation(TargetCharacter->GetActorLocation(), 100);
	}
}

void ADroidAIController::SetTargetLocation()
{
	TArray<AActor*> ClassActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainCharacter::StaticClass(), ClassActors);

	float ShortestDistance = 100000000000000000.0f;
	AMainCharacter *ClosestCharacter = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("Num : %s"), *FString::FromInt(ClassActors.Num()));

	if (GetPawn() == nullptr) { return; }

	for (AActor* Actor : ClassActors) {
		AMainCharacter *Character = Cast<AMainCharacter>(Actor);
		if (Character != nullptr && !Character->Dead) {
			float Distance = (Character->GetActorLocation() - GetPawn()->GetActorLocation()).Size();
			if (Distance < ShortestDistance) {
				ShortestDistance = Distance;
				ClosestCharacter = Character;
			}
		}
	}

	TargetCharacter = ClosestCharacter;

}
