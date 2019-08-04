// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameMechanics/PlayingGameMode.h"
#include "GameMechanics/LobbyGameMode.h"
#include "UI/NetworkUI/MainMenu.h"
#include "GameMechanics/PlatformerGameInstance.h"
#include "GamePlayerController.generated.h"
/**
 * 
 */
class UInGameHUD;
class UKickedNote;
class UPlatformerGameInstance;

UCLASS()
class S08_NETWORKINGCOURSE_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

AGamePlayerController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Tick(float DeltaTime);
	virtual void BeginPlay();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UMainMenu *MainMenuWidget;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Setup_Client();
	UFUNCTION(Client, Reliable, BlueprintCallable)
		void Print(bool Found);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UInGameHUD *InGameHUDReference;

	void AddWidgetToViewport(UKickedNote *WidgetToAdd);

	UPROPERTY(Replicated)
	FRoundData RoundData;

	UFUNCTION(NetMulticast, unreliable)
		void SetRoundData(class AGamePlayerController *PC, FRoundData DataToSet);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UMenu *Menu;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class UMenu> MenuClass;
	UFUNCTION(Client, reliable)
		void SetPlayerDataClient(FPlayerData LoadedData);

	float TimeInitiated;

	UPlatformerGameInstance *GI;

	bool WaitingToSet = false;

	TMap<FString, bool> PendingItems;
	FPlayerData PendingOwningPlayerData;

	bool HasLoadedData = false;


};
