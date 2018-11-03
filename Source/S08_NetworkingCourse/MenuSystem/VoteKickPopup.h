// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "VoteKickPopup.generated.h"

/**
 * 
 */
USTRUCT()
struct FKickInformation {
	GENERATED_BODY()
public:
	FText PlayerName;
	APlayerController *KickingPlayerController;
	FText Reason;
};

UCLASS()
class S08_NETWORKINGCOURSE_API UVoteKickPopup : public UUserWidget
{
	GENERATED_BODY()

private:
	APlayerController *KickingPlayer;

public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock *Player;

	UPROPERTY(meta = (BindWidget))
		UTextBlock *Reason;

	UFUNCTION(BlueprintCallable)
		void RecievePlayerInput(bool Input);

	UFUNCTION()
		void SetInformation(FKickInformation Info);
	
};
