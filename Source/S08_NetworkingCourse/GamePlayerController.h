// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyGameMode.h"
#include "MenuSystem/MainMenu.h"
#include "GamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

	void PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel);

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UMainMenu *MainMenuWidget;
};
