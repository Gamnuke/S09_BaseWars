// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatsTab.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UStatsTab : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *PlayerText;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *ScoreText;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *KillsText;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *AssistsText;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *DeathsText;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *DownsText;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *PingText;
};
