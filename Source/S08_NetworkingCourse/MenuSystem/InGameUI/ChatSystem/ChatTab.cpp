// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatTab.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UChatTab::Setup(FText NewPlayerName, FText NewMessage) {
	if (PlayerName == nullptr) { return; }
	if (Message == nullptr) { return; }
	PlayerName->SetText(NewPlayerName);
	Message->SetText(NewMessage);
}



