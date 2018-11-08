// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "GameFramework/PlayerState.h"
#include "MenuInterface.h"
#include "PauseMenu.generated.h"

class UButton;
class UWidget;
class UWidgetSwitcher;
class UPlayerTab;

UCLASS()
class S08_NETWORKINGCOURSE_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()

	UPauseMenu(const FObjectInitializer& ObjectInitializer);
protected:
	virtual bool Initialize();

private:
	TSubclassOf<class UUserWidget> PlayerTabClass;

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

	/// Vote kick menu

	UPROPERTY(meta = (BindWidget))
		UButton *VoteKickMenuButton;

	UPROPERTY(meta = (BindWidget))
		UButton *VoteKickButton;

	UPROPERTY(meta = (BindWidget))
		UButton *BackButton;

	UPROPERTY(meta = (BindWidget))
		UWidget *VoteKickMenu;

	UPROPERTY(meta = (BindWidget))
		UScrollBox *PlayerBox;

	UPROPERTY(meta = (BindWidget))
		UEditableTextBox *ReasonBox;

	////-----------------
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

	UFUNCTION()
		void OpenVoteKickMenu();

	UFUNCTION()
		void UpdatePlayerTabs();

	TArray<APlayerState*> States;
	TArray<UPlayerTab*> Tabs;
	TOptional<int32> SelectedIndex = NULL;

	void SetMenuInterface(IMenuInterface *NewMenuInterface);

	void SetSelectedPlayerIndex(int32 Index, UPlayerTab *OwningTab);

	UFUNCTION()
	void CallVoteKick();

	bool IsHost = false;
};