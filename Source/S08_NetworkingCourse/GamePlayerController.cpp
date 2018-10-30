// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayerController.h"
#include "GameFramework/PlayerController.h"

void AGamePlayerController::PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) {
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
	if (MainMenuWidget != nullptr) {
		MainMenuWidget->ToLoadingScreen(FText::FromString(FString("Joining Serererre")));
	}
}


