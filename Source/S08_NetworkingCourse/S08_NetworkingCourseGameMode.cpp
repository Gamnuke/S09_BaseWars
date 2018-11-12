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