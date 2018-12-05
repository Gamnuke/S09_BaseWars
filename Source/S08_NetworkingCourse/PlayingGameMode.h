// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API APlayingGameMode : public AGameModeBase
{
	GENERATED_BODY()
		void HandleStartingNewPlayer_Implementation(APlayerController * PlayerController) override;

};
