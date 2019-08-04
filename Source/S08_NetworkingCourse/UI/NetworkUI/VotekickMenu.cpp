// Fill out your copyright notice in the Description page of Project Settings.

#include "VotekickMenu.h"
#include "UI/PlayerTab.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameState.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "ConstructorHelpers.h"

UVotekickMenu::UVotekickMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerTab(TEXT("/Game/UI/PlayerTab_WBP"));
	if (PlayerTab.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *PlayerTab.Class->GetName());
		PlayerTabClass = PlayerTab.Class;
	}

}

bool UVotekickMenu::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }
	//Check if not null

	VoteKickButton->OnClicked.AddDynamic(this, &UVotekickMenu::CallVoteKick);
	RefreshButton->OnClicked.AddDynamic(this, &UVotekickMenu::UpdatePlayerTabs);
	return true;
}

void UVotekickMenu::UpdatePlayerTabs() {
	if (PlayerBox != nullptr) {
		PlayerBox->ClearChildren();
	}
	Tabs.Empty();
	SelectedIndex = NULL;
	States.Empty();

	if (PlayerTabClass != nullptr&&GetWorld() != nullptr) {
		TArray<APlayerState*> Players = GetWorld()->GetGameState()->PlayerArray;
		int32 i = 0;
		for (APlayerState *State : Players) {
			UPlayerTab *Tab = CreateWidget<UPlayerTab>(this, PlayerTabClass, *(FString("PlayerTab") + FString::FromInt(i)));
			Tab->UpdateName(State->GetPlayerName());
			Tab->ParentVotekickMenu = this;
			Tab->ThisIndex = i;
			Tabs.Add(Tab);
			States.Add(State);
			PlayerBox->AddChild(Tab);
			i++;
		}
	}
}

void UVotekickMenu::SetSelectedPlayerIndex(int32 Index, UPlayerTab *OwningTab) {
	for (UPlayerTab *Tab : Tabs) {
		Tab->UpdateColour(false);
	}
	OwningTab->UpdateColour(true);
	SelectedIndex = Index;
	SelectedTab = OwningTab;
}

void UVotekickMenu::CallVoteKick() {
	if (!SelectedIndex.IsSet()) { return; }
	APlayerState* SelectedState = States[SelectedIndex.GetValue()];
	if (SelectedState == nullptr) { return; }
	if (SelectedState->GetNetOwningPlayer() == nullptr) { return; }
	APlayerController *PlayerController = SelectedState->GetNetOwningPlayer()->PlayerController;
	if (PlayerController == nullptr) { return; }
	PlayerController->ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
	if (SelectedTab != nullptr) {
		SelectedTab->RemoveFromViewport();
		SelectedTab = nullptr;
	}
	
}


