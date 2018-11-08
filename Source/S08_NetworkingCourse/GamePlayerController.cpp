// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayerController.h"
#include "GameFramework/PlayerController.h"
#include "MenuSystem/KickedNote.h"

void AGamePlayerController::PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) {
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
	if (MainMenuWidget != nullptr) {
		MainMenuWidget->TearDown();
	}
}

void AGamePlayerController::AddWidgetToViewport(UKickedNote *WidgetToAdd) {
	WidgetToAdd->AddToPlayerScreen();
}


