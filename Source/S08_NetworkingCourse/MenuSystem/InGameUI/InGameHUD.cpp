// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"
#include "ConstructorHelpers.h"
#include "MenuSystem/InGameUI/ChatSystem/ChatTab.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "UnrealNetwork.h"
#include "GamePlayerController.h"
#include "NetworkGameState.h"

UInGameHUD::UInGameHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ChatTab(TEXT("/Game/UI/InGame/ChatUI/ChatTab_WBP"));
	if (ChatTab.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *ChatTab.Class->GetName());
		ChatTabClass = ChatTab.Class;
	}

}

void UInGameHUD::Setup() {
	if (GetOwningPlayer() == nullptr) { return; }
	Owner = Cast<AGamePlayerController>(GetOwningPlayer());
	if (Owner == nullptr) { return; }
	Owner->InGameHUDReference = this;
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

void UInGameHUD::CreateMessage(FText NewPlayerName, FText NewMessage) {
	if (ChatTabClass == nullptr) { return; }
	if (ChatBox == nullptr) { return; }
	UChatTab *NewChatTab = CreateWidget<UChatTab>(this, ChatTabClass, *NewMessage.ToString());
	//NewChatTab->Setup(NewPlayerName, NewMessage);
	ChatBox->AddChild(NewChatTab);
}

void UInGameHUD::FormatChatBox() {
	if (ChatBox->GetChildrenCount() > 5) {
		ChatBox->RemoveChildAt(0);
		ChatBox->ScrollToEnd();
	}
}


