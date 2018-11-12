// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "PlatformerGameInstance.h"
#include "MenuSystem/PlayerTab.h"
#include "ConstructorHelpers.h"
#include "S08_NetworkingCourseGameMode.h"
#include "PlayingGameMode.h"
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
	if (VoteKickButton == nullptr) { return false; }
	if (BackButton == nullptr) { return false; }

	ReturnToGameButton->OnClicked.AddDynamic(this, &UPauseMenu::Teardown);
	MenuButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenMainMenu);
	QuitGameButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenConfirmQuit);
	YesButton->OnClicked.AddDynamic(this, &UPauseMenu::QuitGame);
	NoButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenPauseMenu);
	VoteKickMenuButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenVoteKickMenu);
	BackButton->OnClicked.AddDynamic(this, &UPauseMenu::OpenPauseMenu);
	VoteKickButton->OnClicked.AddDynamic(this, &UPauseMenu::CallVoteKick);


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
	VoteKickButton->bIsEnabled = IsHost;
	VoteKickMenuButton->bIsEnabled = IsHost;
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

void UPauseMenu::OpenVoteKickMenu() {
	if (WidgetSwitcher == nullptr) { return; }
	if (VoteKickMenu == nullptr) { return; }

	WidgetSwitcher->SetActiveWidget(VoteKickMenu);
	UpdatePlayerTabs();
}

void UPauseMenu::UpdatePlayerTabs() {
	if (PlayerBox != nullptr) {
		PlayerBox->ClearChildren();
	}
	Tabs.Empty();
	SelectedIndex = NULL;

	if (PlayerTabClass != nullptr&&GetWorld()!=nullptr) {
		TArray<APlayerState*> Players = GetWorld()->GetGameState()->PlayerArray;
		int32 i = 0;
		for (APlayerState *State : Players) {
			UPlayerTab *Tab = CreateWidget<UPlayerTab>(this, PlayerTabClass, *(FString("PlayerTab") + FString::FromInt(i)));
			Tab->UpdateName(State->GetPlayerName());
			Tab->ParentPauseMenu = this;
			Tab->ThisIndex = i;
			Tabs.Add(Tab);
			States.Add(State);
			PlayerBox->AddChild(Tab);
			i++;
		}
	}
}

void UPauseMenu::SetSelectedPlayerIndex(int32 Index, UPlayerTab *OwningTab) {
	for (UPlayerTab *Tab : Tabs) {
		Tab->UpdateColour(false);
	}
	OwningTab->UpdateColour(true);
	SelectedIndex = Index;
}

void UPauseMenu::CallVoteKick() {
	APlayerState* SelectedState = States[SelectedIndex.GetValue()];
	if (!SelectedIndex.IsSet()) { return; }
	if (SelectedState->GetNetOwningPlayer() == nullptr) { return; }
	APlayerController *PlayerController = SelectedState->GetNetOwningPlayer()->PlayerController;
	if (PlayerController == nullptr) { return; }
	PlayerController->ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green, FString(TEXT("Gamemode Isnt Null")));
}


