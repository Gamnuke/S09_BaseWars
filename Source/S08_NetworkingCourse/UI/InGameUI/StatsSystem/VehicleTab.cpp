// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleTab.h"
#include "Objects/Parts/VehicleConstructor.h"
#include "Components/Button.h"
#include "UI/NetworkUI/Menu.h"

bool UVehicleTab::Initialize() {
	bool Success = Super::Initialize();
	SaveVehicleButton->OnClicked.AddDynamic(this, &UVehicleTab::SaveVehicle);
	LoadVehicleButton->OnClicked.AddDynamic(this, &UVehicleTab::LoadVehicle);

	//GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(643, 100, FColor::Red, FPaths::AutomationDir());
	//UE_LOG(LogTemp, Warning, TEXT("%s"), FPlatformProcess::UserSettingsDir());

	//FString FileSearch = FString(FPlatformProcess::UserDir()).LeftChop(10) + FString("Documents/My Games/Xordia/Vehicles/_Hello_There_") + FString(".txt");
	////if (!FPaths::FileExists(FileSearch)) {
	//FString FileContent = FString();
	//FFileHelper::SaveStringToFile(FileContent, *FileSearch, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_None);

		//FBufferArchive BufferArchive;
		//FPlayerData newPlayerData = FPlayerData();
		//newPlayerData.Items = Items;
		//SaveGameDataToFile(FileSearch, BufferArchive, newPlayerData);
	return true;
}

void UVehicleTab::SaveVehicle()
{
	
}

void UVehicleTab::LoadVehicle()
{
	if (ParentMenu == nullptr) { return; }
	FVehicleData Data;
	ParentMenu->LoadVehicleData(FilePath, Data);
}
