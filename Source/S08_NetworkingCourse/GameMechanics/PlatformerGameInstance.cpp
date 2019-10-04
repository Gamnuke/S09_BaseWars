// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformerGameInstance.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

#include "UI/NetworkUI/MainMenu.h"
#include "UI/InGameUI/PauseMenu.h"
#include "UI/NetworkUI/SessionTab.h"
#include "UI/NetworkUI/ServerStatus.h"
#include "UI/NetworkUI/Menu.h"
#include "UI/InGameUI/HUD/InGameHUD.h"

#include "Public/OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/OnlineSession.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameSession.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameMechanics/GamePlayerController.h"
#include "OnlineSubsystemSteam.h"

#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Misc/FileHelper.h"
#include "AdvancedSessions/Classes/AdvancedSessions.h"
#include "AdvancedSessions/Classes/AdvancedSessionsLibrary.h"
#include "Paths.h"
#include "Objects/Parts/Part.h"
#include "HAL/FileManagerGeneric.h"

const static FName BUILD_SESSION_NAME = NAME_GameSession;
const static FName SESSION_SETTINGS_KEY = TEXT("ServerName");
const static FName SESSION_SETTINGS_Locked = TEXT("ServerLocked");

// Open game
// Connects to the main server

UPlatformerGameInstance::UPlatformerGameInstance(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> UserWidget(TEXT("/Game/UI/MainMenu_WBP"));
	if (UserWidget.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *UserWidget.Class->GetName());
		MenuClass = UserWidget.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenu(TEXT("/Game/UI/InGame/PauseMenu_WBP"));
	if (PauseMenu.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *PauseMenu.Class->GetName());
		PauseMenuClass = PauseMenu.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameHUD(TEXT("/Game/UI/InGame/InGameHUD_WBP"));
	if (InGameHUD.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *InGameHUD.Class->GetName());
		InGameHUDClass = InGameHUD.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> ServerStatus(TEXT("/Game/UI/Lobby/ServerStatus_WBP"));
	if (ServerStatus.Class != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *ServerStatus.Class->GetName());
		ServerStatusClass = ServerStatus.Class;
	}

	ValidateItems();

	//UE_LOG(LogTemp, Warning, TEXT("Files Found: %s"), *Files);
}

void UPlatformerGameInstance::ValidateItems()
{
	TArray<FString> FileDirs;
	IFileManager::Get().FindFilesRecursive(FileDirs, *(FPaths::ProjectContentDir() + FString("Items/")), TEXT("*.uasset"), true, false, false);
	FString Files;

	for (FString Dir : FileDirs) {
		//Files += FPaths::GetBaseFilename(Dir) + FString(", ");
		////FPaths::GetBaseFilename(Dir, false);

		FString SearchDir = Dir;
		SearchDir.RemoveFromStart(FPaths::GetPath(FPaths::ProjectContentDir()));
		SearchDir.RemoveFromEnd(FString(".uasset"));

		//UE_LOG(LogTemp, Warning, TEXT("PENDING SEARCH: %s"), *(FString("/Game") + SearchDir));

		ConstructorHelpers::FClassFinder<APart> Part(*(FString("/Game") + SearchDir));
		if (Part.Class != NULL)
		{
			//UE_LOG(LogTemp, Warning, TEXT("FOUND %s"), *FPaths::GetBaseFilename(Dir));
			//UE_LOG(LogTemp, Warning, TEXT("Pended path %s"), *(FString("/Game") + SearchDir));
			Files += FPaths::GetBaseFilename(Dir) + FString(", ");

			FString PartName = Part.Class.GetDefaultObject()->GetName();

			PartName.RemoveFromStart("Default__");
			PartName.RemoveFromEnd("_C");

			//Items.Add(PartName, Part.Class.GetDefaultObject()->DefaultLocked);
			//Parts.Add(Part.Class, );
			TArray<FString> Files;

			PartsInCategory.Add(Part.Class, Part.Class.GetDefaultObject()->Category);
			NameForPart.Add(PartName, Part.Class);
			Items.Add(PartName, Part.Class.GetDefaultObject()->DefaultLocked);

			//FString FinalPath = (FPaths::ConvertRelativePathToFull(FPaths::GameContentDir()).LeftChop(1) + SearchDir);
			//FinalPath.RemoveFromEnd(*FPaths::GetBaseFilename(Dir));

			//FFileManagerGeneric::Get().FindFiles(Files, *FinalPath);
			////UE_LOG(LogTemp, Warning, TEXT("Found File: %s"), *Part.Class.GetDefaultObject()->GetName());
			//FString GlobalFilesFound;
			//for (FString FileName : Files) {
			//	GlobalFilesFound += FileName + FString(", ");
			//}
			FString Ref;
			GetStringFromEnum(Part.Class.GetDefaultObject()->Category, Ref);
			UE_LOG(LogTemp, Warning, TEXT("Part: %s, Category: %s, Status: %s"), *Part.Class.GetDefaultObject()->GetName(), *Ref, *(Part.Class.GetDefaultObject()->DefaultLocked?FString("Locked"): FString("Unlocked")));
			
		}
	}
}
void UPlatformerGameInstance::GetStringFromEnum_Implementation(ESubCategory Enum, FString & StringRef) {}

void UPlatformerGameInstance::Init() {
	IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem: %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPlatformerGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPlatformerGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPlatformerGameInstance::OnFindBuildSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPlatformerGameInstance::OnJoinBuildSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No subsystem found"));
	}
}

/// Level blueprint calls to load the Main Menu
void UPlatformerGameInstance::LoadMenuWidget() {
	if (MenuClass == nullptr) { return; }
	MenuWidget = CreateWidget<UMainMenu>(this, MenuClass, FName("Menu Widget"));
	if (MenuWidget != nullptr) {
		MenuWidget->Setup();
		//MenuWidget->SetMenuInterface(this);
		MenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

/// Tears down all widgets to make it ready for game/build servers
void UPlatformerGameInstance::SetupGame() {
	if (MenuWidget != nullptr) {
		MenuWidget->TearDown(nullptr);
	}

	if (PauseMenuWidget != nullptr) {
		PauseMenuWidget->Teardown();
	}
}

//-----------|Join Server Functionality|-----------//
void UPlatformerGameInstance::JoinBuildSession(int32 Index) {
	if (!SessionSearch.IsValid()) { return; }
	if (!SessionInterface.IsValid()) { return; }
	UE_LOG(LogTemp, Warning, TEXT("ONJINJNJINJOINJIONOJn"));
	SessionInterface->JoinSession(0, BUILD_SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UPlatformerGameInstance::OnJoinBuildSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
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
		MenuWidget->TearDown(false);
	}

	FInputModeGameAndUI InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;

	MenuWidget->PlayerIsHost = false;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute, true);
}

//-----------|Host Server Functionality|-----------//
/// Destroys the existing session and creates a new session
void UPlatformerGameInstance::HostServer(FText ServerName) {
	DesiredServerName = ServerName.ToString();
	if (SessionInterface.IsValid()) {
		auto *ExistingSession = SessionInterface->GetNamedSession(BUILD_SESSION_NAME);
		if(ExistingSession != nullptr){
			//GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Orange, FString(ExistingSession->SessionName.ToString().Append(TEXT("- Found existing session, destroying..."))));
			SessionInterface->DestroySession(BUILD_SESSION_NAME);
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
		Settings.NumPublicConnections = 100;

		Settings.Set(SESSION_SETTINGS_Locked, false, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		Settings.bUsesPresence = true;
		GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Green, FString("Creating Session...."));

		Settings.Set(SESSION_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, BUILD_SESSION_NAME, Settings);
	}
}

void UPlatformerGameInstance::OnDestroySessionComplete(FName SessionName, bool Success) {
	//GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green, FString(SessionName.ToString().Append(TEXT("- Successfully destroyed session"))));
	CreateSession();
}
void UPlatformerGameInstance::OnCreateSessionComplete(FName SessionName, bool Success) {
	if (!Success) {
		//GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, FString(SessionName.ToString().Append(TEXT("- Failed to create session"))));
		return;
	}
	//GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green, FString(SessionName.ToString().Append(TEXT("- Successfully created session"))));
	MenuWidget->PlayerIsHost = true;
	MenuWidget->TearDown(true);
	//GetEngine()->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green, FString(TEXT("Hosting Server")));
	GetWorld()->ServerTravel("/Game/Levels/Lobby?listen");
}

//-----------|Find servers Functionality|-----------//
void UPlatformerGameInstance::FindBuildSessions() {
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Orange, FString("Finding sessions"));

	if (SessionSearch.IsValid()) {
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionSearch->MaxSearchResults = 100;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		FindSessionStartTime = GetWorld()->GetTimeSeconds();

		MenuWidget->SetMenuState(EMenuState::Searching);
	}
}
void UPlatformerGameInstance::OnFindBuildSessionComplete(bool Success) {
	//UE_LOG(LogTemp, Warning, TEXT("Finished Session Search, time taken: %f"), (-FindSessionStartTime + GetWorld()->GetTimeSeconds()));

	if (Success && SessionSearch.IsValid() && MenuWidget != nullptr) {
		if (SessionSearch->SearchResults.Num() > 0) {
			GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Orange, FString("Found a session"));
			for (FOnlineSessionSearchResult SessionS : SessionSearch->SearchResults) {
				if (SessionS.Session.NumOpenPublicConnections > 0) {
					JoinBuildSession(0);
					break;
				}
			}
			MenuWidget->SetMenuState(EMenuState::Joining);

		}
		else {
			GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Orange, FString("No sessions found"));
			MenuWidget->SetMenuState(EMenuState::Failed);

		}
		//MenuWidget->UpdateSessionTabs(SessionSearch);
	}
	else {
		GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Red, FString("Find session FAILED"));

	}
}

// Opens the pause game menu
void UPlatformerGameInstance::OpenPauseMenu() {
	if (PauseMenuWidget == nullptr && PauseMenuClass != nullptr) {
		PauseMenuWidget = CreateWidget<UPauseMenu>(this, PauseMenuClass, FName("Pause Menu Widget"));
	}
	if (PauseMenuWidget != nullptr) {
		//PauseMenuWidget->SetMenuInterface(this);
		PauseMenuWidget->Setup();
		if (MenuWidget != nullptr) {
			PauseMenuWidget->IsHost = MenuWidget->PlayerIsHost;
		}
	}
}

// Travels the client to the main menu level (Literally exiting the game)
void UPlatformerGameInstance::OpenMainMenu() {
	APlayerController *PlayerController = GetFirstLocalPlayerController();
	if (PlayerController != nullptr) {
		if (PauseMenuWidget != nullptr) {
			PauseMenuWidget->Teardown();
		}
		PlayerController->ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute, false);
	}
}

// Travels the GAME server to a map
void UPlatformerGameInstance::StartGame() {
	//GetEngine()->AddOnScreenDebugMessage(0, 15.0f, FColor::Green, FString("/Game/Levels/" + FPaths::GetBaseFilename(SelectedLevelURL)));
	GetWorld()->ServerTravel(FString("/Game/Levels/Maps/" + FPaths::GetBaseFilename(SelectedLevelURL) + FString("?listen")), true);
}


//-----Build server functionality-----//
bool UPlatformerGameInstance::OnClientJoin_Validate(APlayerController *PC) { return true; }
void UPlatformerGameInstance::OnClientJoin_Implementation(APlayerController *PC)
{

	//GetEngine()->AddOnScreenDebugMessage(0, 10, FColor::Orange, FString("PLAYER JOINED!!!"));
	//UAdvancedSessionsLibrary::GetUniqueNetID(PC, BPNetID);
	FString NetIDString;
	//UAdvancedSessionsLibrary::UniqueNetIdToString(BPNetID, NetIDString);
	GetPlayerUniqueID(PC, NetIDString);
	NetIDString = FString("DevID");

	FString FileSearch = FString(FPlatformProcess::UserDir()).LeftChop(10) + FString("Documents/ServerData/") + NetIDString + FString(".txt");
	if (!FPaths::FileExists(FileSearch)) {
		FString FileContent = FString();
		FFileHelper::SaveStringToFile(FileContent, *FileSearch, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

		FBufferArchive BufferArchive;
		FPlayerData newPlayerData = FPlayerData();
		newPlayerData.Items = Items;
		SaveGameDataToFile(FileSearch, BufferArchive, newPlayerData);

		Cast<AGamePlayerController>(PC)->SetPlayerDataClient(newPlayerData);

		// Go through files on the server of all the items and save them into a file.
		// Give that file to the client so the client knows what's locked and what's unlocked.
		// In the case of a client trying to hack that file, validate it once the vehicle has been built, when the player wants to play a game, or do it periodically(WITHBAND EXPENSIVE).

		// DEBUG to see if the order of the file search changes, as it can be detrimental when writing in binary.
		// Should populate an array of TRUE of FALSE values for whether the item is locked or unlocked.

		// For debug purposes, go through files now and print them in a list.

	}
	else {
		// This player that joined has already played the game, so load his data.
		// In the case of an update:
		// Go through the file on the variable ITEMS and check which does not match with the servers.
		// This is all done on the client itself.

		//** Will check if only the data for the part is available, not if the part itself is included in the player's files. This 
		//can be avoided by allowing players with a specific game version to join.
		FPlayerData LoadedData;
		if (LoadGameDataFromFile(FileSearch, LoadedData)) {
			// On client:
			for (TPair<FString, bool> ServerItemPair : Items)
			{
				if (!LoadedData.Items.Contains(ServerItemPair.Key)) {
					LoadedData.Items.Add(ServerItemPair.Key, ServerItemPair.Value); // Adds the missing item
				}
			}

			Cast<AGamePlayerController>(PC)->SetPlayerDataClient(LoadedData);

		}
	}
}

//--------Purchase functionality--------//
bool UPlatformerGameInstance::UnlockPart_Validate(const FString &PartToUnlock, APlayerController *PC) { 
	return true;
}
void UPlatformerGameInstance::UnlockPart_Implementation(const FString &PartToUnlock, APlayerController *PC)
{
	FString NetIDString;
	//UAdvancedSessionsLibrary::UniqueNetIdToString(BPNetID, NetIDString);
	GetPlayerUniqueID(PC, NetIDString);
	NetIDString = FString("DevID");

	FString FileSearch = FString(FPlatformProcess::UserDir()).LeftChop(10) + FString("Documents/ServerData/") + NetIDString + FString(".txt");
	if (FPaths::FileExists(FileSearch)) {
		Cast<AGamePlayerController>(PC)->Print(true);

		FPlayerData LoadedData;
		if (LoadGameDataFromFile(FileSearch, LoadedData)) {
			// On client:
			//LoadedData.Items
			TSubclassOf<APart> FoundPart = NameForPart.FindRef(PartToUnlock);
			bool ItemLocked = LoadedData.Items.FindRef(PartToUnlock);

			if (FoundPart != nullptr && ItemLocked) {

				FPartStats Stats = FoundPart.GetDefaultObject()->Details;
				if (Stats.Cost <= LoadedData.GB) {
					LoadedData.Items.Emplace(PartToUnlock, false);
					LoadedData.GB = LoadedData.GB - Stats.Cost;

					GetEngine()->AddOnScreenDebugMessage(7, 100, FColor::Green, *(LoadedData.Items.Contains(PartToUnlock) ? FString("Found!") : FString("Not found")));

					FBufferArchive BufferArchive;
					SaveGameDataToFile(FileSearch, BufferArchive, LoadedData);
					Cast<AGamePlayerController>(PC)->SetPlayerDataClient(LoadedData);
				}
				else {
				}
			}
		}
	}
}

//-----Data save and load functionality-----//
void UPlatformerGameInstance::SaveLoadData(FArchive& Ar,FPlayerData& DataToSaveLoad)
{
	Ar << DataToSaveLoad.Experience;
	Ar << DataToSaveLoad.Items;
	Ar << DataToSaveLoad.Level;
	Ar << DataToSaveLoad.GB;
}

bool UPlatformerGameInstance::SaveGameDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary, FPlayerData& DataToSaveLoad)
{
	//note that the supplied FString must be the entire Filepath
	// 	if writing it out yourself in C++ make sure to use the \\
	// 	for example:

	// 	FString SavePath = "C:\\MyProject\\MySaveDir\\mysavefile.save";

	//Step 1: Variable Data -> Binary

	//following along from above examples
	SaveLoadData(ToBinary, DataToSaveLoad);
	//presumed to be global var data, 
	//could pass in the data too if you preferred

	//No Data
	if (ToBinary.Num() <= 0) return false;
	//~

	//Step 2: Binary to Hard Disk
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();

		return true;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	return false;
}

bool UPlatformerGameInstance::LoadGameDataFromFile(
	const FString& FullFilePath,
	FPlayerData& DataToSaveLoad
) {
	//Load the data array,
	// 	you do not need to pre-initialize this array,
	//		UE4 C++ is awesome and fills it 
	//		with whatever contents of file are, 
	//		and however many bytes that is
	TArray<uint8> TheBinaryArray;
	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *FullFilePath))
	{
		return false;
		//~~
	}

	//File Load Error
	if (TheBinaryArray.Num() <= 0) return false;

	//~
	//		  Read the Data Retrieved by GFileManager
	//~

	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);
	SaveLoadData(FromBinary, DataToSaveLoad);

	//~
	//								Clean up 
	//~
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	TheBinaryArray.Empty();
	FromBinary.Close();

	return true;
}

void UPlatformerGameInstance::GetPlayerUniqueID_Implementation(APlayerController *PlayerController, FString& UniqueID) {

}




