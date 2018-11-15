// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerTab.h"
#include "GameFramework/PlayerController.h"

bool UPlayerTab::Initialize() {
	bool Success = Super::Initialize();
	if (Tab != nullptr) {
		Tab->OnClicked.AddDynamic(this, &UPlayerTab::SelectIndex);
	}
	return true;
}

void UPlayerTab::UpdateName(FString Name) {
	if (PlayerNameBlock != nullptr) {
		PlayerNameBlock->SetText(FText::FromString(Name));
	}
}

void UPlayerTab::SelectIndex() {
	if (ParentPauseMenu != nullptr) {
		ParentPauseMenu->SetSelectedPlayerIndex(ThisIndex, this);
	}
	if (AssignedPlayerState == nullptr) { return; }
	if (AssignedPlayerState->GetNetOwningPlayer() == nullptr) { return; }
	APlayerController *PlayerController = AssignedPlayerState->GetNetOwningPlayer()->PlayerController;
	if (PlayerController == nullptr) { return; }
	PlayerController->ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);


}


