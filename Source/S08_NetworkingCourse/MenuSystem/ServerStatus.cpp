// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerStatus.h"
#include "Engine/World.h"
#include "PlatformerGameInstance.h"
#include "Engine/Engine.h"
#include "MenuSystem/PlayerTab.h"


UServerStatus::UServerStatus(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerTab(TEXT("/Game/UI/PlayerTab_WBP"));
	if (PlayerTab.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *PlayerTab.Class->GetName());
		PlayerTabClass = PlayerTab.Class;
	}

}

bool UServerStatus::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }
	if (StartGameButton == nullptr) { return false; }
	StartGameButton->OnClicked.AddDynamic(this, &UServerStatus::StartGame);
	return true;
}

void UServerStatus::Setup() {
	this->AddToPlayerScreen();
	APlayerController *PlayerController = GetOwningPlayer();
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	this->SetVisibility(ESlateVisibility::Visible);
}

void UServerStatus::TearDown() {
	FInputModeGameOnly InputMode;
	GetOwningPlayer()->bShowMouseCursor = false;
	GetOwningPlayer()->SetInputMode(InputMode);
	this->RemoveFromViewport();
}

void UServerStatus::UpdatePlayers(TArray<APlayerState*> PlayerStates) {
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Cyan, FString("Updating Players!"));
	PlayerBox->ClearChildren();
	if (PlayerTabClass != nullptr&&GetWorld() != nullptr) {
		int32 i = 0;
		for (APlayerState *State : PlayerStates) {
			UPlayerTab *Tab = CreateWidget<UPlayerTab>(this, PlayerTabClass, *(FString("PlayerTab") + FString::FromInt(i)));
			Tab->UpdateName(State->GetPlayerName());
			Tab->ThisIndex = i;
			PlayerBox->AddChild(Tab);
			i++;
		}
	}
}

void UServerStatus::StartGame() {
	if (GetWorld() != nullptr) {
		UPlatformerGameInstance* GameInstance = Cast<UPlatformerGameInstance>(GetGameInstance());
		if (GameInstance != nullptr) {
			GameInstance->UpdateSessionSettings();
		}
		TearDown();
		GetWorld()->ServerTravel("/Game/Levels/GameMap?listen");
	}
}

