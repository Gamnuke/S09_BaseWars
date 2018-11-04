// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MenuSystem/PauseMenu.h"
#include "PlayerTab.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UPlayerTab : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Function")
	void UpdateColour(bool Active);
	
	UPROPERTY(meta = (BindWidget))
		UTextBlock *PlayerNameBlock;

	UPROPERTY(meta = (BindWidget))
		UButton *Tab;

	void UpdateName(FString Name);

	int32 ThisIndex;

	UPauseMenu *ParentPauseMenu;

	UFUNCTION()
	void SelectIndex();
};
