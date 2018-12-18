// Fill out your copyright notice in the Description page of Project Settings.

#include "Selector.h"
#include "Components/VerticalBox.h"
#include "MenuSystem/SelectorTab.h"

void USelector::SetSelectedIndex(int32 NewIndex) {
	for (USelectorTab* CurrentTab : CurrentTabs) {
		if (CurrentTab != nullptr) {
			CurrentTab->SetSelectedColor(false);
		}
	}

	if (CurrentTabs[NewIndex] != nullptr) {
		CurrentTabs[NewIndex]->SetSelectedColor(true);
	}
	SelectedIndex = NewIndex;
}

void USelector::PopulateBox(int32 Items) {
	if (List == nullptr) { return; }
	if (SelectorTabClass == nullptr) { return; }

	CurrentTabs.Empty();
	List->ClearChildren();
	for (int32 i = 0; i < Items; i++) {
		USelectorTab* NewTab = CreateWidget<USelectorTab>(this, SelectorTabClass, FName());
		NewTab->AssignedIndex = i;
		NewTab->ParentSelector = this;

		CurrentTabs.Add(NewTab);
		List->AddChild(NewTab);
	}
}



