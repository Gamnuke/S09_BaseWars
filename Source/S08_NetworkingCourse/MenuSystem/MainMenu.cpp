// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "MenuInterface.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Sockets/Public/IPAddress.h"

#include "Public/OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MenuSystem/SessionTab.h"

#include "GamePlayerController.h"
#include "PlatformerGameInstance.h"

#include "ConstructorHelpers.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> SessionTab(TEXT("/Game/UI/SessionTab_WBP"));
	if (SessionTab.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *SessionTab.Class->GetName());
		SessionTabClass = SessionTab.Class;
	}

}

bool UMainMenu::Initialize() {
	bool Success = Super::Initialize();
	if (!Success) { return false; }

	if (ToHostServerMenuButton == nullptr) { return false; }
	ToHostServerMenuButton->OnClicked.AddDynamic(this, &UMainMenu::ToHostMenu);

	if (JoinMenuButton == nullptr) { return false; }
	JoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (JoinServerButton == nullptr) { return false; }
	JoinServerButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);
	
	if (BackButton == nullptr) { return false; }
	BackButton->OnClicked.AddDynamic(this, &UMainMenu::ToStartMenu);

	if (RetryButton == nullptr) { return false; }
	RetryButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (EnterAddressButton == nullptr) { return false; }
	EnterAddressButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (CancelJoinButton == nullptr) { return false; }
	CancelJoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (FetchServerButton == nullptr) { return false; }
	FetchServerButton->OnClicked.AddDynamic(this, &UMainMenu::FetchServers);

	if (BackToMenuButton == nullptr) { return false; }
	BackToMenuButton->OnClicked.AddDynamic(this, &UMainMenu::ToStartMenu);

	if (HostServerButton == nullptr) { return false; }
	HostServerButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (StartGameButton == nullptr) { return false; }
	StartGameButton->OnClicked.AddDynamic(this, &UMainMenu::StartGame);

	AGamePlayerController *GameController =	Cast<AGamePlayerController>(GetOwningPlayer());
	if (GameController != nullptr) {
		GameController->MainMenuWidget = this;
	}
	return true;
}

void UMainMenu::SetMenuInterface(IMenuInterface* NewMenuInterface) {
	MenuInterface = NewMenuInterface;

}

void UMainMenu::HostServer() {
	if (MenuInterface == nullptr) { return; }
	if (ServerNameBlock == nullptr) { return; }
	if (ServerNameBlock->GetText().ToString().IsEmpty()) { return; }
	MenuInterface->HostServer(ServerNameBlock->GetText());
}

void UMainMenu::JoinServer() {
	if (MenuInterface == nullptr) { return; }

	if (SelectedIndex.IsSet()) {
		UE_LOG(LogTemp, Warning, TEXT("Index: %d"), SelectedIndex.GetValue());
		if (MenuInterface != nullptr) {
			USessionTab *Tab = SessionTabs[SelectedIndex.GetValue()];
			if (Tab != nullptr && !Tab->IsLocked) {
				MenuInterface->JoinServer(SelectedIndex.GetValue());
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Please select a session"));
	}

}

void UMainMenu::Setup() {
	this->AddToPlayerScreen();
	APlayerController *PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
	this->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenu::TearDown() {
	APlayerController *PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) { return; }
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false;
	this->SetVisibility(ESlateVisibility::Collapsed);
}

void UMainMenu::OpenJoinMenu() {
	if (WidgetSwitcher == nullptr) { return; }
	if (JoinMenu == nullptr) { return; }
	WidgetSwitcher->SetActiveWidget(JoinMenu);
	PendingHandle.Invalidate();
}

void UMainMenu::ToStartMenu() {
	if (WidgetSwitcher == nullptr) { return; }
	if (StartMenu == nullptr) { return; }
	WidgetSwitcher->SetActiveWidget(StartMenu);
}

void UMainMenu::ToHostMenu() {
	if (WidgetSwitcher == nullptr) { return; }
	if (HostMenu == nullptr) { return; }
	WidgetSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::OpenPendingJoinMenu() {
	if (WidgetSwitcher == nullptr) { return; }
	if (PendingJoin == nullptr) { return; }
	WidgetSwitcher->SetActiveWidget(PendingJoin);
	GetWorld()->GetTimerManager().SetTimer(PendingHandle, this, &UMainMenu::OpenJoinFailedMenu, 10);
}

void UMainMenu::OpenJoinFailedMenu() {
	if (WidgetSwitcher == nullptr) { return; }
	if (JoinFailed == nullptr) { return; }
	WidgetSwitcher->SetActiveWidget(JoinFailed);
}

void UMainMenu::UpdateSessionTabs(TSharedPtr<FOnlineSessionSearch> SearchResult) {
	UE_LOG(LogTemp, Warning, TEXT("Updating Session Tabs!"));
	Status->SetActiveWidget(FoundServers); // Found servers
	ServerList->ClearChildren();
	SelectedIndex = NULL;
	SessionTabs.Empty();

	if (SessionTabClass != nullptr && ServerList != nullptr && ServerList != nullptr) {
		int32 i = 0;
		for (FOnlineSessionSearchResult Element : SearchResult->SearchResults) {
			USessionTab *Tab = CreateWidget<USessionTab>(this, SessionTabClass, *(FString("SessionTab") + FString::FromInt(i)));
			FServerData Data;
			Data.MaxPlayers = Element.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - Element.Session.NumOpenPublicConnections;
			Data.InstantaneousPing = Element.PingInMs;
			Data.HostName = Element.Session.OwningUserId.ToSharedRef().Get().ToString();
			FString ServerName;
			if (Element.Session.SessionSettings.Get(TEXT("ServerName"), ServerName)) {
				Data.ServerName = ServerName;
			}
			bool ServerLocked;
			if (Element.Session.SessionSettings.Get(TEXT("ServerLocked"), ServerLocked)) {
				Data.ServerLocked = ServerLocked;
				if (Tab->LockedImage != nullptr) {
					Tab->LockedImage->Visibility = ServerLocked?ESlateVisibility::Visible:ESlateVisibility::Hidden;
				}
			}
			
			Tab->Setup(this, i, Data);
			SessionTabs.Add(Tab);
			ServerList->AddChild(Tab);
			++i;
		}
	}
}

void UMainMenu::FetchServers() {
	Status->SetActiveWidget(Fetching); // Start fetching servers
	UE_LOG(LogTemp, Warning, TEXT("Fetching Servers...."));

	if (MenuInterface != nullptr) {
		MenuInterface->FindServers();
	}
}

void UMainMenu::SetIndex(USessionTab *OwningTab, int32 Index) {
	SelectedIndex = Index;
	for (USessionTab *SessionTab : SessionTabs) {
		SessionTab->SetActive(false);
	}
	OwningTab->SetActive(true);
}

void UMainMenu::CreateTab() {
	for (int32 i = 0; i < 50; i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Tab"));
		USessionTab *Tab = CreateWidget<USessionTab>(this, SessionTabClass, FName(*FString("Tab:" + FString::FromInt(i))));
		FServerData Data;
		Data.ServerName = TEXT("TESTTESTTESTSTETESTESTSETSETSETSETSETSETSETSETSETSETSETSETSEt");
		Data.MaxPlayers = 0;
		Data.CurrentPlayers = 0;
		Data.InstantaneousPing = 0;
		Data.HostName = TEXT("Testing");
		Tab->Setup(this, i, Data);
		SessionTabs.Add(Tab);
		ServerList->AddChild(Tab);
	}
}

void UMainMenu::ToLoadingScreen(FText Message) {
	this->SetVisibility(ESlateVisibility::Visible);
	WidgetSwitcher->SetActiveWidget(LoadingMenu);
	LoadingMenuText->SetText(Message);
	this->AddToViewport(0);
	
	if (GetGameInstance()->GetEngine() != nullptr){
		GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(0, 15.0f, FColor::Yellow, TEXT("YEEEEEEEEEET"));
	}
}

void UMainMenu::ToServerStatus_Host() {
	MenuInterface->OpenServerStatusMenu();
	this->TearDown();
}

void UMainMenu::StartGame() {
	if (GetWorld() != nullptr) {
		UPlatformerGameInstance* GameInstance = Cast<UPlatformerGameInstance>(GetGameInstance());
		if (GameInstance != nullptr) {
			GameInstance->UpdateSessionSettings();
		}
		TearDown();
		GetWorld()->ServerTravel("/Game/Levels/GameMap?listen");
	}
}

void UMainMenu::ToServerStatus_Client() {
	if (WidgetSwitcher == nullptr) { return; }
}




