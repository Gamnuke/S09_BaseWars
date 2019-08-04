// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"

#include "MusicTab.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UMusicTab : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize();

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
		class UButton *WholeButton;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
		class UTextBlock *MusicName;

	class USoundWave *AssignedSoundWave;
	FString AssignedMusicDir;

	class UMusicControllerUI *ParentController;
	
	UFUNCTION()
	void Select();

};
