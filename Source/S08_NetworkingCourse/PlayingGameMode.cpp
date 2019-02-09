// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayingGameMode.h"
#include "PlatformerGameInstance.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Gameplay/Droid.h"
#include "Gameplay/MainCharacter.h"
#include "Gameplay/ActiveGameState.h"
#include "MenuSystem/InGameUI/InGameHUD.h"
#include "NavigationSystem.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GamePlayerController.h"

#include "Kismet/GameplayStatics.h"

APlayingGameMode::APlayingGameMode() {
	PrimaryActorTick.bCanEverTick = true;
}

void APlayingGameMode::HandleStartingNewPlayer_Implementation(APlayerController * PlayerController)
{
	Super::HandleStartingNewPlayer_Implementation(PlayerController);
}

void APlayingGameMode::HandleSeamlessTravelPlayer(AController *& Controller)
{
	Super::HandleSeamlessTravelPlayer(Controller);
	CurrentPlayers += 1;
	IOnlineSessionPtr Sessions = IOnlineSubsystem::Get()->GetSessionInterface();
	FOnlineSessionSettings* CurrentSettings = Sessions->GetSessionSettings(NAME_GameSession);
}

void APlayingGameMode::BeginPlay()
{
	Super::BeginPlay();
	IOnlineSessionPtr Sessions = IOnlineSubsystem::Get()->GetSessionInterface();
	FOnlineSessionSettings* CurrentSettings = Sessions->GetSessionSettings(NAME_GameSession);

	if (CurrentSettings == nullptr) { return; }
	MaxPlayers = CurrentSettings->NumPublicConnections - Sessions.Get()->GetNamedSession(NAME_GameSession)->NumOpenPublicConnections;

	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(1, 100, FColor::Orange, FString::FromInt(MaxPlayers));
}

void APlayingGameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	float TimeNow = GetWorld()->GetTimeSeconds();

	//if no enemies are left
	//set enemy count to 100
	//set next round start time to Now +10

	//when round starts, set enemies left to spawn to 100 and spawn enemies

	TArray<AActor*> CharacterArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGamePlayerController::StaticClass(), CharacterArray);
	//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(6, 100, FColor::Magenta , FString("Actors found: ") + FString::FromInt(CharacterArray.Num()));

	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(1, 100, FColor::Orange, FString::SanitizeFloat(RoundData.NextRoundBeginTime));
	
	if (RoundData.EnemiesLeft == 0 && RoundData.EnemiesLeftToSpawn == 0 && !RoundData.Intermission) {
		RoundData.NextRoundBeginTime = TimeNow + RoundIntervalTime;
		RoundData.Intermission = true;
	}
	if (TimeNow > RoundData.NextRoundBeginTime && RoundData.EnemiesLeftToSpawn == 0 && RoundData.EnemiesLeft == 0) {
		RoundData.CurrentRound += 1;
		RoundData.EnemiesLeftToSpawn = 4 + FMath::RoundToInt(RoundData.CurrentRound * EnemyCountIncreaseMultiplier);
		RoundData.EnemiesLeft = RoundData.EnemiesLeftToSpawn;
		RoundData.MaxEnemiesThisRound = RoundData.EnemiesLeftToSpawn;
		RoundData.Intermission = false;
	}

	if (TimeNow > RoundData.NextSpawnTime && RoundData.EnemiesLeftToSpawn > 0) {

		
		if (CharacterArray.Num() <= 0) { return; }
		AActor *RandomCharacter = CharacterArray[FMath::RandRange(0, (CharacterArray.Num() - 1))];
		if (RandomCharacter == nullptr) { return; }
		FVector RandomPoint = CharacterArray[FMath::RandRange(0, CharacterArray.Num() - 1)]->GetActorLocation();
		FNavLocation ResultLocation;
		UNavigationSystemV1::GetNavigationSystem(GetWorld())->GetRandomReachablePointInRadius(RandomPoint, 10000.0f, ResultLocation);
		ADroid *Droid = GetWorld()->SpawnActor<ADroid>(DroidClass, ResultLocation.Location, FRotator::ZeroRotator);
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(1, 100, FColor::Orange, FString::FromInt(RoundData.EnemiesLeftToSpawn));
		if (Droid != nullptr) {
			int RNG = FMath::RandRange(0, 20);
			if (RNG == 0) {
				Droid->DroidState.Sprinting = true;
			}

			RoundData.NextSpawnTime = TimeNow + TimeBetweenSpawns;
			RoundData.EnemiesLeftToSpawn -= 1;
		}

		
	}
}

void APlayingGameMode::CountKill()
{
	RoundData.EnemiesLeft -= 1;
	RoundData.EnemiesLeft = FMath::Clamp(RoundData.EnemiesLeft, 0, 10000000);
}


