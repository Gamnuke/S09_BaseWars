// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Selector.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API USelector : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		class UVerticalBox *List;
	
public:
	UFUNCTION()
		void SetSelectedIndex(int32 NewIndex);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TSubclassOf<class UUserWidget> SelectorTabClass;

	TArray<class USelectorTab*> CurrentTabs;
	void PopulateBox(int32 Array);

	TOptional<int32> SelectedIndex;
};
