// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicController.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Objects/MusicController/MusicControllerUI.h"
#include "Objects/MusicController/Visualizer.h"
#include "Gameplay/PlayerCharacter/MainCharacter.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"
#include "MediaSoundComponent.h"
#include "MediaPlayer.h"
#include "FileHelper.h"

// Sets default values
AMusicController::AMusicController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UWidgetComponent>(FName("Root"));
	SetRootComponent(Root);

	Display = CreateDefaultSubobject<UWidgetComponent>(FName("Display"));
	Display->SetupAttachment(Root);
	
	Base = CreateDefaultSubobject<UStaticMeshComponent>(FName("Base"));
	Base->SetupAttachment(Root);

	MediaSoundComponent = CreateDefaultSubobject<UMediaSoundComponent>(FName("MediaSoundComponent"));
	MediaSoundComponent->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AMusicController::BeginPlay()
{
	Super::BeginPlay();
	
	if (Display->GetUserWidgetObject() == nullptr) { return; }
	DisplayUI = Cast<UMusicControllerUI>(Display->GetUserWidgetObject());
	DisplayUI->OwningController = this;
}

// Called every frame
void AMusicController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMusicController::Interact(AMainCharacter * Character)
{
	Super::Interact(Character);

	if (Character == nullptr || Character->Controller == nullptr) { return; }
	FInputModeUIOnly InputMode;
	UMusicControllerUI *ControllerUI = Cast<UMusicControllerUI>(Display->GetUserWidgetObject());
	if (ControllerUI == nullptr) { return; }
	InputMode.SetWidgetToFocus(ControllerUI->TakeWidget());
	APlayerController *C = Cast<APlayerController>(Character->Controller);

	if (C == nullptr) { return; }
	C->SetInputMode(InputMode);
	C->bShowMouseCursor = true;
}

void AMusicController::SetWaveFile(USoundWave * Music)
{
	SelectedMusic = Music;
	/*SelectedMusic->SetGenerating(true);
	SelectedMusic->BeginCachePlatformData();*/
}

void AMusicController::OnFinishedGenerate() {
	
}

void AMusicController::PlayMusic()
{
	if (SelectedMusic != nullptr) {
		TArray < uint8 > rawFile;

		FFileHelper::LoadFileToArray(rawFile, (SelectedMusicDir).GetCharArray().GetData());
		FString rawFileData;

		FWaveModInfo WaveInfo;
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(126, 100, FColor::Orange, (WaveInfo.ReadWaveInfo(rawFile.GetData(), rawFile.Num())) ? FString("Success") : FString("Failed"));
		if (WaveInfo.ReadWaveInfo(rawFile.GetData(), rawFile.Num()))
		{

			USoundWave *NewSoundWave = NewObject<USoundWave>(USoundWave::StaticClass());
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

			TArray<AActor*> OutVisualizers = TArray<AActor*>();
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVisualizer::StaticClass(), OutVisualizers);
			if (OutVisualizers.Num() != 0) {
				int32 i = 0;
				for (AActor* Visualizer : OutVisualizers) {
					AVisualizer *V = Cast<AVisualizer>(Visualizer);
					if (V != nullptr) {
						if (i == 0) {
							V->AffectsFOV = true;
						}
						else {
							V->AffectsFOV = false;
						}
						V->StartTime = GetWorld()->GetTimeSeconds();
						V->CurrentMusic = NewSoundWave;
						i++;
					}
				}
			}

			if (MediaSoundComponent != nullptr && MediaSoundComponent->GetMediaPlayer() != nullptr && SelectedMusicDir != FString()) {
				MediaSoundComponent->GetMediaPlayer()->OpenUrl(FString("file://") + SelectedMusicDir);
			}
		}
	}
}

void AMusicController::OnAfterPlayMusicDelayed()
{
	
}

void AMusicController::OnPlayMusicDelay_Implementation()
{
	
}


