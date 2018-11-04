// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "MenuSystem/PauseMenu.h"
#include "MenuSystem/SessionTab.h"

#include "Public/OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "PlatformerGameInstance.generated.h"

class UMainMenu;
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

	void UpdateSessionSettings();

	UPROPERTY()
	UMainMenu *MenuWidget;

	UPROPERTY()
	UPauseMenu *PauseMenuWidget;

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

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FString DesiredServerName;

	FOnlineSessionSettings Settings;

};
