// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicControllerUI.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Sound/SoundWave.h"
#include "FileHelper.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Objects/MusicController/MusicTab.h"
#include "Objects/MusicController/MusicController.h"
#include "Components/AudioComponent.h"
#include "SoundMod.h"
//#include "Lumin/LuminPlatformProcess.h"

bool UMusicControllerUI::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }

	if (RefreshButton == nullptr) { return false; }
	RefreshButton->OnPressed.AddDynamic(this, &UMusicControllerUI::RefreshMusic);

	if (SelectButton == nullptr) { return false; }
	SelectButton->OnPressed.AddDynamic(this, &UMusicControllerUI::SelectMusic);

	if (ExitButton == nullptr) { return false; }
	ExitButton->OnPressed.AddDynamic(this, &UMusicControllerUI::Exit);
	return true;
}

void UMusicControllerUI::RefreshMusic() {
	/*TArray<USoundWave*> Waves = GetSoundWaveFromFile();

	if (MusicTabClass == nullptr || Waves.Num()==0) { return; }
	int32 i = 0;
	MusicScrollBox->ClearChildren();
	for (USoundWave* Wave : Waves) {
		UMusicTab *NewTab = CreateWidget<UMusicTab>(this, MusicTabClass, FName(*FString::FromInt(i)));
		if (NewTab != nullptr) {
			NewTab->ParentController = this;
			NewTab->AssignedSoundWave = Wave;
			if (MusicNames.IsValidIndex(i)) {
				NewTab->MusicName->SetText(FText::FromString(MusicNames[i]));
			}
			MusicScrollBox->AddChild(NewTab);
		}
		i++;
	}*/
	GetSoundWaveFromFile();
}

void UMusicControllerUI::SelectMusic() {
	if (OwningController != nullptr) {
		OwningController->PlayMusic();
	}
}

void UMusicControllerUI::Exit()
{
	FInputModeGameOnly InputMode;
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->bShowMouseCursor = false;
}

TArray<USoundWave*> UMusicControllerUI::GetSoundWaveFromFile()
{
	TArray<FString> WaveFileDirs;
	TArray<USoundWave*> SoundWaves;
	MusicNames.Empty();
	MusicScrollBox->ClearChildren();
	FString MusicDir = FString(FPlatformProcess::UserDir()).LeftChop(10) + FString("Music/");

	IFileManager::Get().FindFilesRecursive(WaveFileDirs, *MusicDir, TEXT("*.wav"), true, false, false);
	int iii = 0;
	for (FString FileDir : WaveFileDirs) {

		TArray < uint8 > rawFile;

		FFileHelper::LoadFileToArray(rawFile, (MusicDir + FPaths::GetCleanFilename(FileDir)).GetCharArray().GetData());
		FString rawFileData;

		FWaveModInfo WaveInfo;
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(126, 100, FColor::Orange, (WaveInfo.ReadWaveInfo(rawFile.GetData(), rawFile.Num())) ? FString("Success") : FString("Failed"));
		if (WaveInfo.ReadWaveInfo(rawFile.GetData(), rawFile.Num()))
		{

			USoundWave *NewSoundWave = NewObject<USoundWave>(USoundWave::StaticClass());


			SoundWaves.Add(NewSoundWave);
			MusicNames.Add(FPaths::GetBaseFilename(FileDir));

			NewSoundWave->InvalidateCompressedData();

			NewSoundWave->RawData.Lock(LOCKSTATUS_ReadWriteLock);
			void* LockedData = NewSoundWave->RawData.Realloc(rawFile.Num());
			FMemory::Memcpy(LockedData, rawFile.GetData(), rawFile.Num());
			NewSoundWave->RawData.Unlock();

			int32 DurationDiv = *WaveInfo.pChannels * *WaveInfo.pBitsPerSample * *WaveInfo.pSamplesPerSec;
			if (DurationDiv)
			{
				NewSoundWave->Duration = *WaveInfo.pWaveDataSize * 8.0f / DurationDiv;
			}
			else
			{
				NewSoundWave->Duration = 0.0f;
			}
			NewSoundWave->SetSampleRate(*WaveInfo.pSamplesPerSec);
			NewSoundWave->NumChannels = *WaveInfo.pChannels;
			NewSoundWave->RawPCMDataSize = WaveInfo.SampleDataSize;
			NewSoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Default;

			UMusicTab *NewTab = CreateWidget<UMusicTab>(this, MusicTabClass, FName(*FString::FromInt(iii)));
			if (NewTab != nullptr) {
				NewTab->ParentController = this;
				NewTab->AssignedSoundWave = NewSoundWave;
				NewTab->AssignedMusicDir = FileDir;
				NewTab->MusicName->SetText(FText::FromString(FPaths::GetBaseFilename(FileDir)));
				MusicScrollBox->AddChild(NewTab);
			}
		}
		iii++;
	}

	/*int32 iii = 0;
	MusicScrollBox->ClearChildren();
	for (USoundWave* Wave : SoundWaves) {
		UMusicTab *NewTab = CreateWidget<UMusicTab>(this, MusicTabClass, FName(*FString::FromInt(iii)));
		if (NewTab != nullptr) {
			NewTab->ParentController = this;
			NewTab->AssignedSoundWave = Wave;
			if (WaveFileDirs.IsValidIndex(iii)) {
				NewTab->AssignedMusicDir = WaveFileDirs[iii];
			}
			if (MusicNames.IsValidIndex(iii)) {
				NewTab->MusicName->SetText(FText::FromString(MusicNames[iii]));
			}
			MusicScrollBox->AddChild(NewTab);
		}
		iii++;
	}*/
	return SoundWaves;
}
