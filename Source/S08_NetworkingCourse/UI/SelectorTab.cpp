// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectorTab.h"
#include "UI/Selector.h"

#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "UI/NetworkUI/Menu.h"

bool USelectorTab::Initialize() {
	bool Success = Super::Initialize();

	if (Tab != nullptr) {
		//Tab->OnClicked.AddDynamic(this, &USelectorTab::SelectIndex);
		Tab->OnClicked.AddDynamic(this, &USelectorTab::SelectItem);
		Tab->OnHovered.AddDynamic(this, &USelectorTab::OnHovered);
		Tab->OnUnhovered.AddDynamic(this, &USelectorTab::OnUnHovered);

	}

	return true;
}

void USelectorTab::SelectItem() {
	if (ParentSelector != nullptr) {
		ParentSelector->SelectItem(AssignedPart);
	}
}

void USelectorTab::ConstructWidget() {
	NameBlock->SetText(Name);
}


void USelectorTab::SetSelectedColor(bool Active) {
	if (Selected == nullptr) { return; }
	Selected->SetBrushColor(Active ? FLinearColor(0, 1, 1, 1) : FLinearColor(0, 0, 0, 0));
}

void USelectorTab::OnHovered() {
	Hovered = true;

	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(3, 1000, FColor::Red, ParentSelector==nullptr?FString("Selector is null"): FString("Selector is not null"));

	if (ParentSelector != nullptr) {
		ParentSelector->DetailsPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
		ParentSelector->SetDetails(AssignedPart);
	}
}
void USelectorTab::OnUnHovered() {
	Hovered = false;
	ParentSelector->DetailsPanel->SetVisibility(ESlateVisibility::Collapsed);
}


