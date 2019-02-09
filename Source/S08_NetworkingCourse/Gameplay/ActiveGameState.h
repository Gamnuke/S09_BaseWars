// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PlayingGameMode.h"
#include "ActiveGameState.generated.h"

/**
 * 
 */

UCLASS()
class S08_NETWORKINGCOURSE_API AActiveGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	AActiveGameState(const FObjectInitializer& ObjectInitializer);
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	virtual void Tick(float DeltaTime);
public:
	UPROPERTY(Replicated, BlueprintReadWrite, VisibleAnywhere)
	FRoundData RoundData;
};
