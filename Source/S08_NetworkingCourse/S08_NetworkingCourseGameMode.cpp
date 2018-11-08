// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "S08_NetworkingCourseGameMode.h"
#include "S08_NetworkingCourseCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameState.h"
#include "MenuSystem/KickedNote.h"
#include "PlatformerGameInstance.h"
#include "GamePlayerController.h"
#include "Blueprint/UserWidget.h"


AS08_NetworkingCourseGameMode::AS08_NetworkingCourseGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> KickedNoteClass(TEXT("/Game/UI/KickedNote_WBP"));
	if (KickedNoteClass.Class != NULL)
	{
		DefaultKickedNoteClass = KickedNoteClass.Class;
	}
}

void AS08_NetworkingCourseGameMode::KickPlayerCall(int32 KickingPlayerID) {
	TArray<APlayerState*> AllPlayerStates = GameState->PlayerArray;
	for (APlayerState* State : AllPlayerStates) {
		if (State->PlayerId == KickingPlayerID) {
			if (State->GetNetOwningPlayer() == nullptr) { return; }
			if (State->GetNetOwningPlayer()->PlayerController == nullptr) { return; }
			/*if (DefaultKickedNoteClass == nullptr) { return; }

			if (DefaultKickedNoteClass != nullptr) {
				UKickedNote *KickedNoteWidget = CreateWidget<UKickedNote>(State->GetNetOwningPlayer()->PlayerController, DefaultKickedNoteClass, FName("KickedNoteclass"));
				KickedNoteWidget->AddToViewport();
			}*/
			CreateKickNotewidget(State->GetNetOwningPlayer()->PlayerController);
			/*if (PC == nullptr) { return; }
			PC->AddWidgetToViewport(KickedNote);*/
			//State->GetNetOwningPlayer()->PlayerController->ClientTravel("/Game/Levels/MainMenu", ETravelType::TRAVEL_Absolute);
			//GameSession->KickPlayer(State->GetNetOwningPlayer()->PlayerController, FText::FromString(FString("kicked for no reason lol")));
		}
	}
}
