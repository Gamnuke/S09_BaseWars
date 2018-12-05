// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatDisplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UChatDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UScrollBox *ChatBox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *PlayerName;
	
};
