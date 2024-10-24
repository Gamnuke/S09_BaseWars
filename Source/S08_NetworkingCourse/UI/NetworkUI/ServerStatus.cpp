// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerStatus.h"
#include "Engine/World.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "Engine/Engine.h"
#include "UI/PlayerTab.h"


bool UServerStatus::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }
	if (StartGameButton == nullptr) { return false; }
	StartGameButton->OnClicked.AddDynamic(this, &UServerStatus::StartGame);
	return true;
}

void UServerStatus::Setup() {
	this->AddToPlayerScreen();
	APlayerController *PlayerController = GetOwningPlayer();
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	this->SetVisibility(ESlateVisibility::Visible);
}

void UServerStatus::TearDown() {
	FInputModeGameOnly InputMode;
	GetOwningPlayer()->bShowMouseCursor = false;
	GetOwningPlayer()->SetInputMode(InputMode);
	this->RemoveFromViewport();
}

void UServerStatus::StartGame() {
	if (GetWorld() != nullptr) {
		UPlatformerGameInstance* GameInstance = Cast<UPlatformerGameInstance>(GetGameInstance());
		if (GameInstance != nullptr) {
			TearDown();
			GameInstance->StartGame();
		}
	}
}

