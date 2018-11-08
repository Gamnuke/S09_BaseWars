// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "S08_NetworkingCourseGameMode.generated.h"

UCLASS(minimalapi)
class AS08_NetworkingCourseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AS08_NetworkingCourseGameMode();
	void KickPlayerCall(int32 KickingPlayerID);

	UFUNCTION(BlueprintImplementableEvent)
		void CreateKickNotewidget(APlayerController *OwningPlayerController);
private:
	TSubclassOf<class UUserWidget> DefaultKickedNoteClass;
};



