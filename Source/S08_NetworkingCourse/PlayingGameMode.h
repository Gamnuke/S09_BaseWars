// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayingGameMode.generated.h"

USTRUCT(BlueprintType)
struct FRoundData {
	GENERATED_BODY();

	UPROPERTY()
		int CurrentRound = 0;

	UPROPERTY()
		int MaxEnemiesThisRound = 0;

	UPROPERTY()
		int EnemiesLeftToSpawn = 0;
	UPROPERTY()
		int EnemiesLeft = 0;
	UPROPERTY()
		float NextRoundBeginTime;
	UPROPERTY()
		float NextSpawnTime;
	UPROPERTY()
		bool Intermission = true;
};

UCLASS()
class S08_NETWORKINGCOURSE_API APlayingGameMode : public AGameModeBase
{
	GENERATED_BODY()
		void HandleStartingNewPlayer_Implementation(APlayerController * PlayerController) override;

	APlayingGameMode();
protected:
	virtual void HandleSeamlessTravelPlayer(AController *&Controller);
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	int MaxPlayers;
	int CurrentPlayers;

public:
	void CountKill();

	FRoundData RoundData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int StartingEnemyCount = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float EnemyCountIncreaseMultiplier = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RoundIntervalTime = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TimeBetweenSpawns = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class ADroid> DroidClass;

};
