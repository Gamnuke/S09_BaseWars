// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"

#include "Public/OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "PlayingGameMode.h"

#include "PlatformerGameInstance.generated.h"

class UMainMenu;
class UServerStatus;
class USessionTab;
class UPauseMenu;
class UInGameHUD;
/**
 * 
 */



UCLASS()
class S08_NETWORKINGCOURSE_API UPlatformerGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

	UPlatformerGameInstance(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Init();

	FString SelectedLevelURL;

	void UpdatePlayerTabs();

	UFUNCTION(exec)
		void JoinServer(int32 Index);
	
	UFUNCTION(exec)
		void HostServer(FText ServerName);

	UFUNCTION(exec)
		void FindServers();

	UFUNCTION(exec)
		void OpenMainMenu();

	UFUNCTION(BlueprintCallable)
		void LoadMenuWidget();

	UFUNCTION(BlueprintCallable)
		void OpenPauseMenu();

	UFUNCTION(exec)
		void OpenServerStatusMenu();

	UFUNCTION()
		void StartGame();


	void UpdateSessionSettings();

	UPROPERTY()
		UMainMenu *MenuWidget;
	UPROPERTY()
		UPauseMenu *PauseMenuWidget;
	UPROPERTY()
		UServerStatus *ServerStatusWidget;
	UPROPERTY()
		UInGameHUD *InGameHUDWidget;

	UFUNCTION(BlueprintCallable)
	void SetupGame();

	UFUNCTION(exec)
		void CreateTab();


	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void CreateSession();
	void SetServerName(FString NewServerName);

	float FindSessionStartTime;
	class AS08_NetworkingCourseGameMode *Gamemode;
private:
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> PauseMenuClass;
	TSubclassOf<class UUserWidget> ServerStatusClass;
	TSubclassOf<class UUserWidget> InGameHUDClass;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FString DesiredServerName;

	FOnlineSessionSettings Settings;

};
