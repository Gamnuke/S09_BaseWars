// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnrealNetwork.h"
#include "InGameHUD.generated.h"

class UVerticalBox;
class UScrollBox;
class AGamePlayerController;

UCLASS()
class S08_NETWORKINGCOURSE_API UInGameHUD : public UUserWidget
{
	GENERATED_BODY()
protected:
	UInGameHUD(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable)
	void ComposeNewMessage(FText NewPlayerName, FText NewMessage);

	void CreateMessage(FText NewPlayerName, FText NewMessage);

	TSubclassOf<class UUserWidget> ChatTabClass;

	void Setup();

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		UScrollBox *ChatBox;
	
	AGamePlayerController *Owner;
};
