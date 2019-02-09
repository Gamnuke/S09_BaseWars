// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DroidAIController.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API ADroidAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADroidAIController();

	virtual void Tick(float DeltaTime) override;

	void SetTargetLocation();

protected:
	virtual void BeginPlay() override;
	float NextSwitchTime;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class AMainCharacter *TargetCharacter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class AActor> CharacterClass;
};
