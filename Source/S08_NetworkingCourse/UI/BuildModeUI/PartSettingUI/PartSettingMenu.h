// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PartSettingMenu.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UPartSettingMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UVerticalBox *DetailContainer;
};
