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
#include "Gameplay/ActiveGameState.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

#include "Kismet/GameplayStatics.h"



AGamePlayerController::AGamePlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	bReplicates = true;
}

void AGamePlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamePlayerController, RoundData);
}

void AGamePlayerController::Setup_Client_Implementation() {
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

}

void AGamePlayerController::AddWidgetToViewport(UKickedNote *WidgetToAdd) {
	WidgetToAdd->AddToPlayerScreen();
}

void AGamePlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
}

void AGamePlayerController::SetRoundData_Implementation(AGamePlayerController *PC, FRoundData DataToSet) {
	RoundData = DataToSet;
}