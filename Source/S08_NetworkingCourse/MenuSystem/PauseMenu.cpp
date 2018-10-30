// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

bool UPauseMenu::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }
	//Check if not null

	if (ReturnToGameButton == nullptr) { return false; }
	if (MenuButton == nullptr) { return false; }
	if (QuitGameButton == nullptr) { return false; }
	if (YesButton == nullptr) { return false; }
	if (NoButton == nullptr) { return false; }

	ReturnToGameButton->OnClicked.AddDynamic(this, &UPauseMenu::Teardown);
	MenuButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenMainMenu);
	QuitGameButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenConfirmQuit);
	YesButton->OnClicked.AddDynamic(this, &UPauseMenu::QuitGame);
	NoButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenPauseMenu);

	return true;
}
void UPauseMenu::Teardown() {
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false;
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
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }

	FInputModeGameAndUI InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	this->AddToPlayerScreen();
}

void UPauseMenu::QuitGame() {
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit);
}

void UPauseMenu::SetMenuInterface(IMenuInterface *NewMenuInterface) {
	MenuInterface = NewMenuInterface;
}



