// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "S08_NetworkingCourseGameMode.h"
#include "S08_NetworkingCourseCharacter.h"
#include "UObject/ConstructorHelpers.h"

AS08_NetworkingCourseGameMode::AS08_NetworkingCourseGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
