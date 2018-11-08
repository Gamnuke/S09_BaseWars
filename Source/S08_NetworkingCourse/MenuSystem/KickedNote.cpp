// Fill out your copyright notice in the Description page of Project Settings.

#include "KickedNote.h"

bool UKickedNote::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }
	if (CloseButton == nullptr) { return false; }
	CloseButton->OnClicked.AddDynamic(this, &UKickedNote::CloseWidget);

	return true;
}

void UKickedNote::CloseWidget() {
	RemoveFromViewport();
	this->Destruct();
}



