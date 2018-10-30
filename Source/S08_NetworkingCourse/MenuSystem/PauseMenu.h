// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "MenuInterface.h"
#include "PauseMenu.generated.h"

class UButton;
class UWidget;
class UWidgetSwitcher;

UCLASS()
class S08_NETWORKINGCOURSE_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize();

private:
	//Widgets//
	UPROPERTY(meta = (BindWidget))
		UButton *ReturnToGameButton;

	UPROPERTY(meta = (BindWidget))
		UButton *MenuButton;

	UPROPERTY(meta = (BindWidget))
		UButton *QuitGameButton;

	UPROPERTY(meta = (BindWidget))
		UButton *YesButton;

	UPROPERTY(meta = (BindWidget))
		UButton *NoButton;

	UPROPERTY(meta = (BindWidget))
		UWidget *PauseMenu;

	UPROPERTY(meta = (BindWidget))
		UWidget *Confirmation;

	UPROPERTY(meta = (BindWidget))
		UWidgetSwitcher *WidgetSwitcher;

	IMenuInterface *MenuInterface;

public:
	//Functions//
	UFUNCTION(BlueprintCallable)
		void OpenPauseMenu();

	UFUNCTION()
		void OpenConfirmQuit();

	UFUNCTION()
		void Teardown();

	UFUNCTION()
		void QuitGame();
	
	UFUNCTION()
		void OpenMainMenu();

	UFUNCTION(BlueprintCallable)
		void Setup();

	UFUNCTION(BlueprintCallable)
		void ClosePauseMenu();

	void SetMenuInterface(IMenuInterface *NewMenuInterface);
};