// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UItemData : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FText ItemName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Locked = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UPlatformerGameInstance *GI;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class APart> AssignedPart;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UMenu *ParentSelector;
};
