// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformerGameInstance.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/PauseMenu.h"
#include "MenuSystem/SessionTab.h"

#include "Public/OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/OnlineSession.h"
#include "GameFramework/GameSession.h"
#include "S08_NetworkingCourseGameMode.h"

const static FName SESSION_NAME = NAME_GameSession;
const static FName SESSION_SETTINGS_KEY = TEXT("ServerName");
const static FName SESSION_SETTINGS_Locked = TEXT("ServerLocked");

UPlatformerGameInstance::UPlatformerGameInstance(const FObjectInitializer& ObjectInitializer)
{

	static ConstructorHelpers::FClassFinder<UUserWidget> UserWidget(TEXT("/Game/UI/MainMenu_WBP"));
	if (UserWidget.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *UserWidget.Class->GetName());
		MenuClass = UserWidget.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenu(TEXT("/Game/UI/PauseMenu_WBP"));
	if (PauseMenu.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *PauseMenu.Class->GetName());
		PauseMenuClass = PauseMenu.Class;
	}
}

void UPlatformerGameInstance::Init() {
	IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem: %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPlatformerGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPlatformerGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPlatformerGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPlatformerGameInstance::OnJoinSessionComplete);
			FindServers();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No subsystem found"));
	}
}

void UPlatformerGameInstance::LoadMenuWidget() {
	if (MenuClass == nullptr) { return; }
	MenuWidget = CreateWidget<UMainMenu>(this, MenuClass, FName("Menu Widget"));
	if (MenuWidget != nullptr) {
		MenuWidget->Setup();
		MenuWidget->SetMenuInterface(this);
		MenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPlatformerGameInstance::SetupGame() {
	if (MenuWidget != nullptr) {
		MenuWidget->TearDown();
	}
}

void UPlatformerGameInstance::JoinServer(int32 Index) {
	if (!SessionSearch.IsValid()) { return; }
	if (!SessionInterface.IsValid()) { return; }
	UE_LOG(LogTemp, Warning, TEXT("ONJINJNJINJOINJIONOJn"));
	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UPlatformerGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	if (!SessionInterface.IsValid()) { return; }
	FString Address; // Reference to the string of address
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address)) {
		UE_LOG(LogTemp, Warning, TEXT("Couldnt get connect string"));
		return;
	}
	if (GetWorld() == nullptr) { return; }

	APlayerController *PlayerController = GetFirstLocalPlayerController(GetWorld());
	if (PlayerController == nullptr) { return; }

	if (MenuWidget != nullptr) {
		MenuWidget->TearDown();
	}
	MenuWidget->PlayerIsHost = false;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPlatformerGameInstance::HostServer(FText ServerName) {
	DesiredServerName = ServerName.ToString();
	if (SessionInterface.IsValid()) {
		auto *ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if(ExistingSession != nullptr){
			GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Orange, FString(ExistingSession->SessionName.ToString().Append(TEXT("- Found existing session, destroying..."))));
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else 
		{
			CreateSession();
		}
	}
}

void UPlatformerGameInstance::CreateSession() {
	if (SessionInterface.IsValid()) {
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") {
			Settings.bIsLANMatch = true;
		}
		else {
			Settings.bIsLANMatch = false;
		}

		Settings.bShouldAdvertise = true;
		if (MenuWidget != nullptr && MenuWidget->PlayerCounter != nullptr) {
			if (MenuWidget->PlayerCounter->GetText().ToString().IsNumeric()) {
				Settings.NumPublicConnections = FCString::Atoi(*MenuWidget->PlayerCounter->GetText().ToString());
			}
		}

		Settings.Set(SESSION_SETTINGS_Locked, false, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		Settings.bUsesPresence = true;
		UE_LOG(LogTemp, Warning, TEXT("Attempting to host server"));
		Settings.Set(SESSION_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, SESSION_NAME, Settings);
	}
}

void UPlatformerGameInstance::UpdateSessionSettings() {
	if (SessionInterface.IsValid()) {
		if (MenuWidget != nullptr && MenuWidget->LockSessionBox != nullptr) {
			Settings.Set(SESSION_SETTINGS_Locked, (MenuWidget->LockSessionBox->CheckedState == ECheckBoxState::Checked), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			SessionInterface.Get()->UpdateSession(SESSION_NAME, Settings, true);
		}
	}
}

void UPlatformerGameInstance::OnCreateSessionComplete(FName SessionName, bool Success) {
	if (!Success) {
		GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, FString(SessionName.ToString().Append(TEXT("- Failed to create session"))));
		return;
	}
	GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green, FString(SessionName.ToString().Append(TEXT("- Successfully created session"))));
	MenuWidget->TearDown();
	MenuWidget->PlayerIsHost = true;
	GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green, FString(TEXT("Hosting Server")));
	GetWorld()->ServerTravel("/Game/Levels/Lobby?listen");
}

void UPlatformerGameInstance::OnDestroySessionComplete(FName SessionName, bool Success) {
	GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green, FString(SessionName.ToString().Append(TEXT("- Successfully destroyed session"))));
	CreateSession();
}

void UPlatformerGameInstance::FindServers() {
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid()) {
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionSearch->MaxSearchResults = 100;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		FindSessionStartTime = GetWorld()->GetTimeSeconds();
	}
}
void UPlatformerGameInstance::OnFindSessionComplete(bool Success) {
	UE_LOG(LogTemp, Warning, TEXT("Finished Session Search, time taken: %f"), (-FindSessionStartTime + GetWorld()->GetTimeSeconds()));

	if (Success && SessionSearch.IsValid() && MenuWidget != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Sessions found: %d"), SessionSearch->SearchResults.Num());
		MenuWidget->UpdateSessionTabs(SessionSearch);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No sessions found"));
	}
}

void UPlatformerGameInstance::OpenPauseMenu() {
	if (PauseMenuWidget == nullptr && PauseMenuClass != nullptr) {
		PauseMenuWidget = CreateWidget<UPauseMenu>(this, PauseMenuClass, FName("Pause Menu Widget"));
	}
	if (PauseMenuWidget != nullptr) {
		PauseMenuWidget->SetMenuInterface(this);
		PauseMenuWidget->Setup();
	}
}

void UPlatformerGameInstance::OpenMainMenu() {
	APlayerController *PlayerController = GetFirstLocalPlayerController();
	if (PlayerController != nullptr) {
		if (PauseMenuWidget != nullptr) {
			PauseMenuWidget->Teardown();
		}
		PlayerController->ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
	}
}

void UPlatformerGameInstance::CreateTab() {
	if (MenuWidget != nullptr) {
		MenuWidget->CreateTab();
	}
}

void UPlatformerGameInstance::SetServerName(FString NewServerName) {
	DesiredServerName = NewServerName;
}
