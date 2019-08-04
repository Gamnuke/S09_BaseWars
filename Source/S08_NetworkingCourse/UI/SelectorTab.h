// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture.h"
#include "SelectorTab.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMapSelection {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FString MapName;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UTexture *MapThumbnail;
};


UCLASS()
class S08_NETWORKINGCOURSE_API USelectorTab : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString AssignedPart;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UButton* Tab;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UBorder *Selected;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UTextBlock *NameBlock;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
		class UImage *ImageBlock;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UMenu *ParentSelector;
	FText Name;
	
protected:
	virtual bool Initialize();

	UFUNCTION()
	void SelectItem();

public:
	void SetSelectedColor(bool Active);
	UFUNCTION()
		void OnHovered();
	UFUNCTION()
		void OnUnHovered();

	void ConstructWidget();
public:
	UPROPERTY()
		bool Hovered;

};
