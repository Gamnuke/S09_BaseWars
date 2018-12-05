// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"

#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/CheckBox.h"

#include "Public/OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "MainMenu.generated.h"

class USessionTab;

USTRUCT()
struct FServerData {
	GENERATED_BODY()
	public:
		int32 CurrentPlayers;
		int32 MaxPlayers;
		int32 InstantaneousPing;
		FString HostName;
		FString ServerName;
		bool ServerLocked;
};

UCLASS()
class S08_NETWORKINGCOURSE_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

	UMainMenu(const FObjectInitializer& ObjectInitializer);

public:
	void SetMenuInterface(IMenuInterface* NewMenuInterface);
	void Setup();
	void TearDown(TOptional<bool> IsHost);
	void UpdateSessionTabs(TSharedPtr<FOnlineSessionSearch> SearchResult);
	void SetIndex(USessionTab *OwningTab, int32 Index);

	UFUNCTION(BlueprintCallable)
		void ToLoadingScreen(FText Message);

	void ToServerStatus_Host();
	void ToServerStatus_Client();

	void CreateTab();

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, VisibleAnywhere)
		class UTextBlock *PlayerCounter;

	UPROPERTY(meta = (BindWidget))
		class UCheckBox *LockSessionBox;

	bool PlayerIsHost = false;

protected:
	virtual bool Initialize();

private:
	//Main Menu
	UPROPERTY(meta = (BindWidget))
		class UButton *ToHostServerMenuButton;
	
	UPROPERTY(meta = (BindWidget))
		class UButton *JoinMenuButton;
//----------------------------------
	
	//Join Menu
	UPROPERTY(meta = (BindWidget))
		class UButton *JoinServerButton;

	UPROPERTY(meta = (BindWidget))
		class UButton *BackButton;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher *WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
		class UWidget *JoinMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget *StartMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget *Fetching;

	UPROPERTY(meta = (BindWidget))
		class UWidget *FoundServers;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher *Status;

	UPROPERTY(meta = (BindWidget))
		class UButton *FetchServerButton;
//----------------------------------

	//Join Menu -> Join Status
	UPROPERTY(meta = (BindWidget))
		class UWidget *PendingJoin;

	UPROPERTY(meta = (BindWidget))
		class UWidget *JoinFailed;
//----------------------------------

	//Join Status Buttons
	UPROPERTY(meta = (BindWidget))
		class UButton *CancelJoinButton;

	UPROPERTY(meta = (BindWidget))
		class UButton *RetryButton;

	UPROPERTY(meta = (BindWidget))
		class UButton *EnterAddressButton;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox *ServerList;
//----------------------------------

	//Main menu -> Host menu
	UPROPERTY(meta = (BindWidget))
		class UWidget *HostMenu;

	UPROPERTY(meta = (BindWidget))
		class UButton *HostServerButton;

	UPROPERTY(meta = (BindWidget))
		class UButton *BackToMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox *ServerNameBlock;

	UPROPERTY(meta = (BindWidget))
		class UWidget *LoadingMenu;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock *LoadingMenuText;


//----------------------------------

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void ToStartMenu();

	UFUNCTION()
	void ToHostMenu();

	UFUNCTION()
	void OpenPendingJoinMenu();

	UFUNCTION()
	void OpenJoinFailedMenu();

	UFUNCTION()
	void FetchServers();

	IMenuInterface* MenuInterface;

	FTimerHandle PendingHandle;

	bool Pending;

	TSubclassOf<class UUserWidget> SessionTabClass;

	TArray<USessionTab*> SessionTabs;

	TOptional<int32> SelectedIndex;
};
