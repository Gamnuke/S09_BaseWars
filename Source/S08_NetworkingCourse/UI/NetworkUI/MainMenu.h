// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "UI/MenuInterface.h"

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

UENUM()
enum EMenuState {
	Failed, Searching, Joining
};

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
	void Setup();
	void TearDown(TOptional<bool> IsHost);

	void ToServerStatus_Client();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool PlayerIsHost = false;

	TArray<FString> MapFiles;

	class UPlatformerGameInstance *GIRef;

	void SetMenuState(EMenuState State);

public:
	UPROPERTY(meta = (BindWidget)) 
		class UTextBlock *Status;
	UPROPERTY(meta = (BindWidget))
		class UButton *RetrySearchButton;
	UPROPERTY(meta = (BindWidget))
		class UCircularThrobber *Throbber;
protected:
	virtual bool Initialize();

private:

//----------------------------------

	UFUNCTION()
	void FindBuildServers();

	FTimerHandle PendingHandle;

	bool Pending;

};
