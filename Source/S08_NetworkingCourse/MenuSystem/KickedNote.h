// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "KickedNote.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UKickedNote : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual bool Initialize();

public:	
	UPROPERTY(meta = (BindWidget))
		UButton *CloseButton;
	
	UPROPERTY(meta = (BindWidget))
		UTextBlock *ReasonBox;

	UFUNCTION()
		void CloseWidget();
};
