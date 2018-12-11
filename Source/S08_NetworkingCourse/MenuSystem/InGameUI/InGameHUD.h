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
	virtual bool Initialize();
public:
	UFUNCTION(BlueprintCallable)
		void ComposeNewMessage(FText NewPlayerName, FText NewMessage);
	UFUNCTION(BlueprintCallable)
		void FormatChatBox();
	UFUNCTION()
		void OnTextCommitted(const FText & Text, ETextCommit::Type CommitType);

	TSubclassOf<class UUserWidget> ChatTabClass;

	void Setup();

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		UScrollBox *ChatBox;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UEditableText *TextInput;

	AGamePlayerController *Owner;
};
