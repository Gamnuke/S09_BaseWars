// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatDisplayTab.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UChatDisplayTab : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, VisibleAnywhere)
		class UBorder *FadeBorder;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, VisibleAnywhere)
		class UBorder *ColorBorder;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, VisibleAnywhere)
		class UTextBlock *Message;
	
	
};
