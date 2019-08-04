// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "UI/MenuInterface.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/CircularThrobber.h"
#include "UI/Selector.h"
#include "Engine/World.h"
#include "Engine/Texture.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Sockets/Public/IPAddress.h"

#include "Public/OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "UI/NetworkUI/SessionTab.h"

#include "GameMechanics/GamePlayerController.h"
#include "GameMechanics/PlatformerGameInstance.h"

#include "HAL/FileManager.h"

#include "ConstructorHelpers.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UMainMenu::SetMenuState(EMenuState State)
{
	switch (State)
	{
	case Failed:
		Status->SetText(FText::FromString("Search Failed"));
		Throbber->SetVisibility(ESlateVisibility::Collapsed);
		RetrySearchButton->SetVisibility(ESlateVisibility::Visible);
		break;
	case Searching:
		Status->SetText(FText::FromString("Searching"));
		Throbber->SetVisibility(ESlateVisibility::Visible);
		RetrySearchButton->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case Joining:
		Status->SetText(FText::FromString("Server Found! Joining..."));
		Throbber->SetVisibility(ESlateVisibility::Collapsed);
		RetrySearchButton->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:
		Status->SetText(FText::FromString(""));
		Throbber->SetVisibility(ESlateVisibility::Collapsed);
		RetrySearchButton->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}

bool UMainMenu::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }

	AGamePlayerController *GameController =	Cast<AGamePlayerController>(GetOwningPlayer());
	if (GameController != nullptr) {
		GameController->MainMenuWidget = this;
	}

	RetrySearchButton->OnClicked.AddDynamic(this, &UMainMenu::FindBuildServers);

	return true;
}

void UMainMenu::Setup() {
	this->AddToPlayerScreen();
	APlayerController *PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	this->SetVisibility(ESlateVisibility::Visible);

	if (GetGameInstance()!=nullptr) {
		GIRef = Cast<UPlatformerGameInstance>(GetGameInstance());
	}

	if (!PlayerIsHost) {
		FindBuildServers();
	}
	else {
		GIRef->CreateSession();
	}
}

void UMainMenu::TearDown(TOptional<bool> IsHost) {
	APlayerController *PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }
	this->RemoveFromViewport();
}

void UMainMenu::FindBuildServers() {
	if (GIRef == nullptr) { return; }
	GIRef->FindBuildSessions(); // Find a build server and then join it.
}

void UMainMenu::ToServerStatus_Client() {
	/*if (GetOwningPlayer()->Role != ROLE_Authority) {
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Orange, FString("PLAYER JOINED ON CLIENT!!!"));
		GIRef->OnClientJoin();
	}*/
}




