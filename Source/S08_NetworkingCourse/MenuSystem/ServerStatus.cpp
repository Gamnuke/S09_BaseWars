// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerStatus.h"
#include "Engine/World.h"


void UServerStatus::Setup() {
	this->AddToPlayerScreen();
	APlayerController *PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	this->SetVisibility(ESlateVisibility::Visible);
}

void UServerStatus::TearDown() {
	
}

void UServerStatus::UpdatePlayers(TArray<APlayerState*> PlayerStates) {

}

