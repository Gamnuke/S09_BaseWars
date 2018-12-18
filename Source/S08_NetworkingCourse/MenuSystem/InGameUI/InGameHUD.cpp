// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"
#include "ConstructorHelpers.h"
#include "MenuSystem/InGameUI/ChatSystem/ChatTab.h"

#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/Border.h"
#include "Components/EditableText.h"

#include "UnrealNetwork.h"
#include "GamePlayerController.h"
#include "NetworkGameState.h"

#include "Gameplay/MainCharacter.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerState.h"

UInGameHUD::UInGameHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ChatTab(TEXT("/Game/UI/InGame/ChatUI/ChatTab_WBP"));
	if (ChatTab.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *ChatTab.Class->GetName());
		ChatTabClass = ChatTab.Class;
	}

}
bool UInGameHUD::Initialize() {
	bool Success = Super::Initialize();
	if (TextInput == nullptr) { return false; }
	TextInput->OnTextCommitted.AddDynamic(this, &UInGameHUD::OnTextCommitted);
	return Success;
}

void UInGameHUD::Setup() {
	if (GetOwningPlayer() == nullptr) { return; }
	Owner = Cast<AGamePlayerController>(GetOwningPlayer());
	if (Owner == nullptr) { return; }
	Owner->InGameHUDReference = this;
}

void UInGameHUD::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if (TextInput == nullptr) { return; }
	if (FadeBorder == nullptr) { return; }
	if (GetOwningPlayer()->WasInputKeyJustPressed(FKey("Y"))) {

		TargetOpacity = 1;
		TextInput->SetKeyboardFocus();
	}
	FLinearColor Color = FadeBorder->ContentColorAndOpacity;
	Color.A = FMath::FInterpTo(
		FadeBorder->ContentColorAndOpacity.A,
		TargetOpacity,
		DeltaTime,
		5
	);

	FadeBorder->SetContentColorAndOpacity(Color);
}

void UInGameHUD::ComposeNewMessage(FText NewPlayerName, FText NewMessage) {
	if (ChatTabClass == nullptr) { return; }
	if (ChatBox == nullptr) { return; }
	UChatTab *NewChatTab = CreateWidget<UChatTab>(this, ChatTabClass, *NewMessage.ToString());
	//NewChatTab->Setup(NewPlayerName, NewMessage);
	ChatBox->AddChild(NewChatTab);
	/*if (Owner !=nullptr) {
		Owner->Server_AddNewMessage(NewPlayerName, NewMessage);
	}*/
}

void UInGameHUD::FormatChatBox() {
	if (ChatBox->GetChildrenCount() > 5) {
		ChatBox->RemoveChildAt(0);
		ChatBox->ScrollToEnd();
	}
}

void UInGameHUD::OnTextCommitted(const FText & Text, ETextCommit::Type CommitType){
	if (CommitType == ETextCommit::OnEnter) {
		TextInput->SetText(FText());

		TargetOpacity = 0;

		APlayerController *Player = GetOwningPlayer();
		if (Player == nullptr) { return; }
		
		if (GetWorld()->GetName() == FString("Lobby")) {
			FInputModeGameAndUI InputMode;
			Player->SetInputMode(InputMode);
			Player->bShowMouseCursor = true;
		}
		else {
			FInputModeGameOnly InputMode;
			Player->SetInputMode(InputMode);
			Player->bShowMouseCursor = false;
		}

		if (Text.ToString() != FString() && GetOwningPlayerPawn() != nullptr) {
			AMainCharacter *Character = Cast<AMainCharacter>(GetOwningPlayerPawn());

			if (Character == nullptr || Player->PlayerState == nullptr) { return; }
			Character->Server_CreateChatDisplay(FText::FromString(Player->PlayerState->GetPlayerName()), Text, Character->AssignedColor);
		}
		ChatBox->ScrollToEnd();

	}
}

