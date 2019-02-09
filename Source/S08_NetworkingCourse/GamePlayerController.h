// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyGameMode.h"
#include "MenuSystem/MainMenu.h"
#include "PlayingGameMode.h"
#include "GamePlayerController.generated.h"

/**
 * 
 */
class UInGameHUD;
class UKickedNote;

UCLASS()
class S08_NETWORKINGCOURSE_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

AGamePlayerController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Tick(float DeltaTime);
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UMainMenu *MainMenuWidget;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Setup_Client();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UInGameHUD *InGameHUDReference;

	void AddWidgetToViewport(UKickedNote *WidgetToAdd);

	UPROPERTY(Replicated)
	FRoundData RoundData;

	UFUNCTION(NetMulticast, unreliable)
		void SetRoundData(class AGamePlayerController *PC, FRoundData DataToSet);
};
