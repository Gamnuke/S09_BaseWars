// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMechanics/GamePlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"
#include "UI/InGameUI/HUD/InGameHUD.h"
#include "GameMechanics/ActiveGameState.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

#include "Kismet/GameplayStatics.h"
#include "UI/NetworkUI/Menu.h"
#include "GameMechanics/PlatformerGameInstance.h"




AGamePlayerController::AGamePlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGamePlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamePlayerController, RoundData);
}

void AGamePlayerController::Setup_Client_Implementation() {
	bShowMouseCursor = true;

	/*if (Menu == nullptr) {
		Menu = CreateWidget<UMenu>(this, MenuClass, FName("MenuUI"));
	}
	Menu->AddToPlayerScreen();
	Cast<UPlatformerGameInstance>(GetGameInstance())->LoadTimeInitiated = GetWorld()->GetTimeSeconds();
	TimeInitiated = GetWorld()->GetTimeSeconds();*/
}

void AGamePlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	
	if (!HasAuthority() && GI!=nullptr && GI->OwningPlayerData.Items.Num() == 0) {
		GI->OnClientJoin(this);
	}
}

void AGamePlayerController::BeginPlay() {
	Super::BeginPlay();
	
		if (GetGameInstance() != nullptr) {
			GI = Cast<UPlatformerGameInstance>(GetGameInstance());
			if (GI == nullptr) { 
				GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(14, 1, FColor::Red, FString("GAME INSTANCE IS NULL OMGMOGOMGOMOGM"));
				return;
			};
		}


	//Setup_Client();

}

	//GI->OnClientJoin(this);


void AGamePlayerController::SetRoundData_Implementation(AGamePlayerController *PC, FRoundData DataToSet) {
	RoundData = DataToSet;
}

void AGamePlayerController::SetPlayerDataClient_Implementation(FPlayerData LoadedData) {
	//OwningPlayerData = LoadedData;
	if (GI != nullptr) {

		GI->OwningPlayerData = LoadedData;
		GI->Items = LoadedData.Items;
	}
	else {
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(13, 1, FColor::Red, FString("GAME INSTANCE IS NULL"));
	}

	if (Menu == nullptr) {
		Menu = CreateWidget<UMenu>(this, MenuClass, FName("MenuUI"));
		Menu->AddToPlayerScreen();
		bShowMouseCursor = true;
	}
	if (Menu != nullptr) {
		Menu->PopulateCategories(Menu->CurrentCategory);
		Menu->GBDisplay->SetText(FText::FromString(FString::FromInt(LoadedData.GB)));
	}
	HasLoadedData = true;
	//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(10, 100, FColor::Green, FString("Time took to load:") + FString::SanitizeFloat(GetWorld()->GetTimeSeconds() - TimeInitiated));
	//Menu->RemoveFromViewport();
	//Cast<AGamePlayerController>(GetFirstLocalPlayerController(GetWorld()))->Menu->RemoveFromViewport();
}

void AGamePlayerController::Print_Implementation(bool Found) {
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(10, 100, FColor::Green, Found?FString("HOLY SHIT I FOUND IT"):FString("Fuck I didnt find it :c"));

}