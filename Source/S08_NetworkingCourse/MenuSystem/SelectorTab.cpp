// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectorTab.h"
#include "MenuSystem/Selector.h"

#include "Components/Button.h"
#include "Components/Border.h"

bool USelectorTab::Initialize() {
	bool Success = Super::Initialize();

	if (Tab != nullptr) {
		Tab->OnClicked.AddDynamic(this, &USelectorTab::SelectIndex);
	}
	return true;
}

void USelectorTab::SelectIndex() {
	if (ParentSelector != nullptr) {
		ParentSelector->SetSelectedIndex(AssignedIndex);
	}
}

void USelectorTab::SetSelectedColor(bool Active) {
	if (Selected == nullptr) { return; }
	Selected->SetBrushColor(Active ? FLinearColor(0, 1, 1, 1) : FLinearColor(0, 0, 0, 0));
}


