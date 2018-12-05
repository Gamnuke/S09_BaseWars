// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayerController.h"
#include "GameFramework/PlayerController.h"
#include "MenuSystem/KickedNote.h"
#include "Engine/GameInstance.h"
#include "PlatformerGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "GamePlayerController.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"
#include "MenuSystem/InGameUI/InGameHUD.h"

void AGamePlayerController::Setup_Client_Implementation() {
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

}

void AGamePlayerController::AddWidgetToViewport(UKickedNote *WidgetToAdd) {
	WidgetToAdd->AddToPlayerScreen();
}

