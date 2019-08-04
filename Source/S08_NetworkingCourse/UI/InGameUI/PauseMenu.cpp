// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "UI/PlayerTab.h"
#include "ConstructorHelpers.h"
#include "GameMechanics/PlayingGameMode.h"
#include "Engine/Engine.h"

UPauseMenu::UPauseMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerTab(TEXT("/Game/UI/PlayerTab_WBP"));
	if (PlayerTab.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *PlayerTab.Class->GetName());
		PlayerTabClass = PlayerTab.Class;
	}

}

bool UPauseMenu::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }
	//Check if not null

	if (ReturnToGameButton == nullptr) { return false; }
	if (MenuButton == nullptr) { return false; }
	if (QuitGameButton == nullptr) { return false; }
	if (YesButton == nullptr) { return false; }
	if (NoButton == nullptr) { return false; }
	if (VoteKickMenuButton == nullptr) { return false; }
	if (BackButton == nullptr) { return false; }

	ReturnToGameButton->OnClicked.AddDynamic(this, &UPauseMenu::Teardown);
	MenuButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenMainMenu);
	QuitGameButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenConfirmQuit);
	YesButton->OnClicked.AddDynamic(this, &UPauseMenu::QuitGame);
	NoButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenPauseMenu);
	VoteKickMenuButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenVoteKickMenu);
	BackButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenPauseMenu);

	return true;
}
void UPauseMenu::Teardown() {
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }

	if (GetWorld() != nullptr) {
		if (GetWorld()->GetName() == FString("Lobby")) {
			FInputModeGameAndUI InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = true;
		}
		else {
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = false;
		}
	}
	
	this->RemoveFromViewport();
}

void UPauseMenu::OpenMainMenu() {
	if (MenuInterface != nullptr) {
		this->Teardown();
		MenuInterface->OpenMainMenu();
	}
}

void UPauseMenu::OpenConfirmQuit() {
	if (WidgetSwitcher == nullptr) { return; }
	if (Confirmation == nullptr) { return; }
	WidgetSwitcher->SetActiveWidget(Confirmation);
}

void UPauseMenu::OpenPauseMenu() {
	if (WidgetSwitcher == nullptr) { return; }
	if (PauseMenu == nullptr) { return; }
	WidgetSwitcher->SetActiveWidget(PauseMenu);
}

void UPauseMenu::ClosePauseMenu() {
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false;
}

void UPauseMenu::Setup() {
	VoteKickMenuButton->bIsEnabled = IsHost;
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	this->AddToPlayerScreen();
}

void UPauseMenu::QuitGame() {
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

void UPauseMenu::SetMenuInterface(IMenuInterface *NewMenuInterface) {
	MenuInterface = NewMenuInterface;
}

void UPauseMenu::OpenVoteKickMenu() {
	if (WidgetSwitcher == nullptr) { return; }
	if (KickMenuContainer == nullptr) { return; }
	if (VoteKickMenu != nullptr) {
		VoteKickMenu->UpdatePlayerTabs();
	}
	WidgetSwitcher->SetActiveWidget(KickMenuContainer);
}


