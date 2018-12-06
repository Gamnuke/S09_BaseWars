// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatDisplayWidget.h"
#include "MenuSystem/InGameUI/ChatSystem/ChatDisplayTab.h"
#include "Components/ScrollBox.h"
#include "ConstructorHelpers.h"

UChatDisplayWidget::UChatDisplayWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ChatDisplayTabSearch(TEXT("/Game/UI/InGame/ChatUI/ChatDisplayTab_WBP"));
	if (ChatDisplayTabSearch.Class != NULL)
	{
		ChatDisplayTabClass = ChatDisplayTabSearch.Class;
	}
}

void UChatDisplayWidget::AddChatTab(FText Message) {
	if (ChatBox == nullptr) { return; }
	UChatDisplayTab *NewTab = nullptr;
	NewTab = CreateWidget<UChatDisplayTab>(this, ChatDisplayTabClass, *FString::FromInt(NewTab->GetUniqueID()));

	if (NewTab != nullptr) {
		ChatBox->AddChild(NewTab);
	}
}



