// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "Engine/World.h"
#include "ConstructorHelpers.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "UI/NetworkUI/MainMenu.h"
#include "GameMechanics/GamePlayerController.h"
#include "PlatformerGameInstance.h"
#include "Engine/Engine.h"
#include "AdvancedSessionsLibrary.h"
#include "FileHelper.h"
#include "Objects/Parts/Part.h"
#include "UI/NetworkUI/Menu.h"

ALobbyGameMode::ALobbyGameMode(const FObjectInitializer& ObjectInitializer) {
	bUseSeamlessTravel = true;
	//static ConstructorHelpers::FClassFinder<UUserWidget> PlayerTab(TEXT("/Game/UI/PlayerTab_WBP"));
	
}
void ALobbyGameMode::BeginPlay(){
	Super::BeginPlay();
	
	/*FString Files;
	for (TSubclassOf<UPart> Part : Parts) {
		Files += Part.GetDefaultObject()->PartName + FString(", ");
	}
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 1000, FColor::Green, FString("Asset files found: ") + Files);*/
}


void ALobbyGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NumOfPlayers += 1;
	Validate();
	bUseSeamlessTravel = true;
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 2, FColor::Orange, FString("A player has joined the lobby"));
	//if (GetNetOwningPlayer()->PlayerController->NetPlayerIndex == 0) {
	if (GetGameInstance() == nullptr) { return; }
	UPlatformerGameInstance *GameInstance = Cast<UPlatformerGameInstance>(GetGameInstance());
	/*if (GameInstance != nullptr && GameInstance->MenuWidget != nullptr) {
		GameInstance->MenuWidget->Setup();
		if (GameInstance->MenuWidget->PlayerIsHost) {
			GameInstance->MenuWidget->ToServerStatus_Host();
		}
		else {
			GameInstance->MenuWidget->ToServerStatus_Client();
			GameInstance->OnClientJoin();
		}
	}*/
		
	if (GetNumPlayers()>1) {
		GameInstance->OnClientJoin(NewPlayer);
	}
	else {
		#if WITH_EDITOR
			GameInstance->OnClientJoin(NewPlayer);
		#endif
	}
		/*FBPUniqueNetId BPNetID;
		UAdvancedSessionsLibrary::GetUniqueNetID(NewPlayer, BPNetID);
		FString NetIDString;
		UAdvancedSessionsLibrary::UniqueNetIdToString(BPNetID, NetIDString);*/
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

