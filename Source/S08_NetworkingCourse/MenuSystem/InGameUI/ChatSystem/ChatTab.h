// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatTab.generated.h"

class UTextBlock;

UCLASS()
class S08_NETWORKINGCOURSE_API UChatTab : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Setup(FText NewPlayerName, FText NewMessage);

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, VisibleAnywhere)
		UTextBlock *PlayerName;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, VisibleAnywhere)
		UTextBlock *Message;
};
