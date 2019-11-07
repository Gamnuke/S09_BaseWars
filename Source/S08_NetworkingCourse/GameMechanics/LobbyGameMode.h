// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	ALobbyGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController * ExitingPlayer) override;
	
private:
	int32 NumOfPlayers;

public:
	int32 MaxPlayers = 3;
	void Validate();

	TArray<TSubclassOf<class UPart>> Parts;

	TSubclassOf<class UUserWidget> MenuUIClass;
};
