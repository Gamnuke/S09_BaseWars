// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMechanics/GamePlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"
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
}

void AGamePlayerController::Setup_Client_Implementation() {
	bShowMouseCursor = true;
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


void AGamePlayerController::SetPlayerDataClient(FPlayerData LoadedData) {
	//OwningPlayerData = LoadedData;
	if (GI != nullptr) {

		GI->OwningPlayerData = LoadedData;
		GI->Items = LoadedData.Items;
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
}
