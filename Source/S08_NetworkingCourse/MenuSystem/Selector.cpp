// Fill out your copyright notice in the Description page of Project Settings.

#include "Selector.h"
#include "Components/ScrollBox.h"
#include "MenuSystem/SelectorTab.h"
#include "Engine/Engine.h"
#include "HAL/FileManager.h"

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

void USelector::PopulateBox_Maps(TArray<FString> MapNames) {
	if (List == nullptr) { return; }
	if (SelectorTabClass == nullptr) { return; }

	CurrentTabs.Empty();
	List->ClearChildren();
	for (int32 i = 0; i < MapNames.Num(); i++) {
		USelectorTab* NewTab = CreateWidget<USelectorTab>(this, SelectorTabClass, FName());
		NewTab->AssignedIndex = i;
		NewTab->ParentSelector = this;
		
		//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 900, FColor::Green, *(FString("/Game/Levels/MapThumbnails/") + FPaths::GetBaseFilename(MapNames[i])));
		/*if (Thumbnail != nullptr) {
			GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 15.0f, FColor::Green, TEXT("Found thumbail!"));
		}*/
		UTexture* Thumbnail = Cast<UTexture>(StaticLoadObject(UTexture::StaticClass(), nullptr, *(FString("/Game/Levels/MapThumbnails/") + FPaths::GetBaseFilename(MapNames[i]))));

		FMapSelection NewMapData;
		NewMapData.MapName = FPaths::GetBaseFilename(MapNames[i]);
		if (Thumbnail != nullptr) {
			NewMapData.MapThumbnail = Thumbnail;
		}

		NewTab->MapData = NewMapData;

		CurrentTabs.Add(NewTab);
		List->AddChild(NewTab);
	}
}



