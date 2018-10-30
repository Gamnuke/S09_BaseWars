// Fill out your copyright notice in the Description page of Project Settings.

#include "SessionTab.h"
#include "Button.h"
#include "TextBlock.h"
#include "MenuSystem/MainMenu.h"
#include "Styling/SlateTypes.h"

bool USessionTab::Initialize() {
	bool Success = Super::Initialize();
	if (Tab != nullptr) {
		Tab->OnClicked.AddDynamic(this, &USessionTab::SelectIndex);
	}
	return true;
}

void USessionTab::Setup(UMainMenu * Parent, int32 Index, FServerData Data)
{
	ThisIndex = Index;
	MainMenuParent = Parent;

	//Set server name
	if (SessionNameBlock != nullptr) {
		SessionNameBlock->SetText(FText::FromString(Data.ServerName));
	}

	//Set ping
	PingBlock->SetText(FText::FromString(FString::FromInt(Data.InstantaneousPing)));

	//Set players
	FString CP = FString::FromInt(Data.CurrentPlayers);
	FString MP = FString::FromInt(Data.MaxPlayers);
	FString Message = CP + "/" + MP;
	PlayersBlock->SetText(FText::FromString(Message));

	//Set host name
	if (HostBlock != nullptr) {
		HostBlock->SetText(FText::FromString(Data.HostName));
	}

	IsLocked = Data.ServerLocked;
}

void USessionTab::SelectIndex() {
	if (MainMenuParent != nullptr) {
		MainMenuParent->SetIndex(this, ThisIndex);
		SetActive(true);
	}
}

void USessionTab::SetActive(bool Active) {
	this->Active = Active;
	this->UpdateColour(Active);
}

