// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatsMenu.generated.h"

/**
 * 
 */
USTRUCT()
struct FPlayerStats {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FText Player;
	UPROPERTY()
		FText Score;
	UPROPERTY()
		FText Kills;
	UPROPERTY()
		FText Assists;
	UPROPERTY()
		FText Deaths;
	UPROPERTY()
		FText Downs;
	UPROPERTY()
		FText Ping;
};

UCLASS()
class S08_NETWORKINGCOURSE_API UStatsMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateStats(FPlayerStats PlayerStats);
};
