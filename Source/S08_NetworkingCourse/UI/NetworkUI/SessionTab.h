// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MenuInterface.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "UI/NetworkUI/MainMenu.h"
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

};
