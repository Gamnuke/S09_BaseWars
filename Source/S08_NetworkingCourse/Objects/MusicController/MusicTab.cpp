// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicTab.h"
#include "Components/Button.h"
#include "Objects/MusicController/MusicControllerUI.h"
#include "Objects/MusicController/MusicController.h"

bool UMusicTab::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }

	if (WholeButton == nullptr) { return false; }
	WholeButton->OnClicked.AddDynamic(this, &UMusicTab::Select);

	return true;
}

void UMusicTab::Select() {
	ParentController->OwningController->SetWaveFile(AssignedSoundWave);
	ParentController->OwningController->SelectedMusicDir = AssignedMusicDir;
}
