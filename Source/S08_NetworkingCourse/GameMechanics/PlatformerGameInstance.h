// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UI/MenuInterface.h"

#include "Public/OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "UI/NetworkUI/Menu.h"
#include "PlatformerGameInstance.generated.h"

class UMainMenu;
class UServerStatus;
class USessionTab;
class UMenu;
/**
 * 
 */

// When an item is clicked, unlock it.
// Make the join unique net id the same.

USTRUCT(Blueprintable)
struct FPlayerData {
	GENERATED_USTRUCT_BODY();

	UPROPERTY()
		TArray<int32> Level;

	UPROPERTY()
		int32 Experience = 0;

	UPROPERTY()
		TMap<FString, bool> Items;

	UPROPERTY()
		int32 GB = 410;
};



UCLASS()
class S08_NETWORKINGCOURSE_API UPlatformerGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPlatformerGameInstance(const FObjectInitializer& ObjectInitializer);

public:
	void ValidateItems();

public:
	virtual void Init();

	FString SelectedLevelURL;

	UFUNCTION(exec)
		void JoinBuildSession(int32 Index);
	
	UFUNCTION(exec)
		void HostServer(FText ServerName);

	UFUNCTION(exec)
		void FindBuildSessions();

	UFUNCTION(exec)
		void OpenMainMenu();

	UFUNCTION(BlueprintCallable)
		void LoadMenuWidget();

	UFUNCTION()
		void StartGame();

	void OnClientJoin_Implementation(APlayerController * PC);

	UFUNCTION(Server, Reliable, WithValidation)
	void OnClientJoin(APlayerController *PC);

	UFUNCTION(BlueprintNativeEvent)
	void GetPlayerUniqueID(APlayerController *PlayerController, FString& UniqueID);

	UPROPERTY()
		UMainMenu *MenuWidget;
	UPROPERTY()
		UServerStatus *ServerStatusWidget;

	UFUNCTION(BlueprintCallable)
	void SetupGame();

	UFUNCTION(BlueprintNativeEvent)
		void GetStringFromEnum(ESubCategory Enum, FString &StringRef);

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindBuildSessionComplete(bool Success);
	void OnJoinBuildSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(exec)
	void CreateSession();

	float FindSessionStartTime;
	class AS08_NetworkingCourseGameMode *Gamemode;

	//For server only
	TMap<FString, bool> Items;
	TMap<TSubclassOf<class UPart>, ESubCategory> CategoryOfStaticPart;
	//TMap<TSubclassOf<class USkeletalPart>, ESubCategory> CategoryOfSkeletalPart;
	TMap<FString, TSubclassOf<class UPart>> NameForStaticPart;
	//TMap<FString, TSubclassOf<class USkeletalPart>> NameForSkeletalPart;

	FPlayerData OwningPlayerData;
	
	UFUNCTION(BlueprintCallable, Server, reliable, WithValidation)
		void UnlockPart(const FString &PartToUnlock, APlayerController *PC);
private:
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> PauseMenuClass;
	TSubclassOf<class UUserWidget> ServerStatusClass;
	TSubclassOf<class UUserWidget> InGameHUDClass;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FString DesiredServerName;

	FOnlineSessionSettings Settings;

public:
	// Save and loading data

	//FArchive is shared base class for FBufferArchive and FMemoryReader
	void SaveLoadData(class FArchive& Ar, FPlayerData& DataToSaveLoad);

	bool SaveGameDataToFile(const FString & FullFilePath, class FBufferArchive & ToBinary, FPlayerData & DataToSaveLoad);

	bool LoadGameDataFromFile(const FString & FullFilePath, FPlayerData & DataToSaveLoad);

	bool IsServer;

	float LoadTimeInitiated = 0;

};
