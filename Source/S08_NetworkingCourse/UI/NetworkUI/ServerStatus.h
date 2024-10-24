// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Widget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "ServerStatus.generated.h"

/**
 * 
 */

UCLASS()
class S08_NETWORKINGCOURSE_API UServerStatus : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	virtual bool Initialize();

public:
	void Setup();
	void TearDown();
	UFUNCTION()
		void StartGame();

	TSubclassOf<class UUserWidget> PlayerTabClass;

private:
	//Hosting -> Host Status
	UPROPERTY(meta = (BindWidget))
		class UWidget *HostStatus;

	UPROPERTY(meta = (BindWidget))
		class UButton *StartGameButton;

	UPROPERTY(meta = (BindWidget))
		class UButton *ShutdownServerButton;

};
