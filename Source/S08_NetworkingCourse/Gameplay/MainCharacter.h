// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MenuSystem/InGameUI/ChatSystem/ChatDisplayWidget.h"

#include "MainCharacter.generated.h"

UCLASS()
class S08_NETWORKINGCOURSE_API AMainCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widgets)
		UChatDisplayWidget *ChatDisplayWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

private:
	void MoveForward(float Value);

	void MoveRight(float Value);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
