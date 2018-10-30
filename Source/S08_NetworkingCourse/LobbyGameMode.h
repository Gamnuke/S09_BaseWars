// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S08_NetworkingCourseGameMode.h"

#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API ALobbyGameMode : public AS08_NetworkingCourseGameMode
{
	GENERATED_BODY()
	ALobbyGameMode(const FObjectInitializer& ObjectInitializer);

	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController * ExitingPlayer) override;

private:
	int32 NumOfPlayers;

public:
	int32 MaxPlayers = 3;
	void Validate();
};
