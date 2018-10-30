// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "MenuSystem/MainMenu.h"
#include "SessionTab.generated.h"

class UButton;
class UTextBlock;
class UMainMenu;
/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API USessionTab : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize();

public:
	UPROPERTY(meta = (BindWidget))
		UButton *Tab;

	UPROPERTY(meta = (BindWidget))
		UTextBlock *SessionNameBlock;
	
	UPROPERTY(meta = (BindWidget))
		UTextBlock *PingBlock;

	UPROPERTY(meta = (BindWidget))
		UTextBlock *PlayersBlock;

	UPROPERTY(meta = (BindWidget))
		UTextBlock *HostBlock;

	UPROPERTY(meta = (BindWidget))
		UImage *LockedImage;

	UFUNCTION(BlueprintImplementableEvent, Category = "Function")
		void UpdateColour(bool Active);

	void Setup(class UMainMenu* Parent, int32 Index, FServerData Data);

	UFUNCTION()
	void SelectIndex();

	void SetActive(bool Active);
	bool Active = false;
	bool IsLocked;

private:
	int32 ThisIndex;
	class UMainMenu* MainMenuParent;

};
