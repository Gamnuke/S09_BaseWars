// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "Engine/World.h"
#include "ConstructorHelpers.h"
#include "PlatformerGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "MenuSystem/MainMenu.h"
#include "GamePlayerController.h"
#include "PlatformerGameInstance.h"

ALobbyGameMode::ALobbyGameMode(const FObjectInitializer& ObjectInitializer) {}

void ALobbyGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	NumOfPlayers += 1;
	Validate();
	bUseSeamlessTravel = true;
	UE_LOG(LogTemp, Warning, TEXT("A player has joined the lobby."));
	//if (GetNetOwningPlayer()->PlayerController->NetPlayerIndex == 0) {
		if (GetGameInstance() == nullptr) { return; }
		UPlatformerGameInstance *GameInstance = Cast<UPlatformerGameInstance>(GetGameInstance());
		if (GameInstance != nullptr && GameInstance->MenuWidget != nullptr) {
			GameInstance->MenuWidget->Setup();
			if (GameInstance->MenuWidget->PlayerIsHost) {
				GameInstance->MenuWidget->ToServerStatus_Host();
			}
			else {
				GameInstance->MenuWidget->ToServerStatus_Client();
			}
		}
	//}
}

void ALobbyGameMode::Logout(AController * ExitingPlayer)
{
	Super::Logout(ExitingPlayer);
	NumOfPlayers += -1;
	Validate();
	UE_LOG(LogTemp, Warning, TEXT("A player has left the lobby."));
}

void ALobbyGameMode::Validate() {
	/*if (NumOfPlayers >= MaxPlayers) {
		UE_LOG(LogTemp, Warning, TEXT("Server full"));

		UWorld *World = GetWorld();
		if (World != nullptr) {
			World->ServerTravel("/Game/Levels/GameMap?listen");
		}
	}*/
}
