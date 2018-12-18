// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectorTab.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API USelectorTab : public UUserWidget
{
	GENERATED_BODY()
	
public:
	int32 AssignedIndex;

	UPROPERTY(meta = (BindWidget))
		class UButton* Tab;
	UPROPERTY(meta = (BindWidget))
		class UBorder *Selected;

	class USelector *ParentSelector;
	
protected:
	virtual bool Initialize();

public:
	void SetSelectedColor(bool Active);

	UFUNCTION()
		void SelectIndex();
};
