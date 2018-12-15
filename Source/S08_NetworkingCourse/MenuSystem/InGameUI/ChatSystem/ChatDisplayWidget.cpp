// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatDisplayWidget.h"
#include "MenuSystem/InGameUI/ChatSystem/ChatDisplayTab.h"
#include "Components/ScrollBox.h"
#include "ConstructorHelpers.h"

#include "Components/TextBlock.h"
#include "Components/Border.h"

UChatDisplayWidget::UChatDisplayWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ChatDisplayTabSearch(TEXT("/Game/UI/InGame/ChatUI/ChatDisplayTab_WBP"));
	if (ChatDisplayTabSearch.Class != NULL)
	{
		ChatDisplayTabClass = ChatDisplayTabSearch.Class;
	}
}

void UChatDisplayWidget::AddChatTab(FText Message, FLinearColor AssignedColor) {
	if (ChatBox == nullptr) { return; }
	UChatDisplayTab *NewTab = nullptr;
	NewTab = CreateWidget<UChatDisplayTab>(this, ChatDisplayTabClass, *Message.ToString());

	if (NewTab != nullptr) {
		ChatBox->AddChild(NewTab);
		NewTab->Message->SetText(Message);
		NewTab->ColorBorder->SetBrushColor(AssignedColor);
	}
}



