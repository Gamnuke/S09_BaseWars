// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MenuSystem/InGameUI/ChatSystem/ChatDisplayWidget.h"
#include "UnrealNetwork.h"
#include "Styling/SlateColor.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent *WidgetComponent;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Widgets)
		class UChatDisplayWidget *ChatDisplayWidget;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Widgets)
		class UInGameHUD *InGameHUD;

	UPROPERTY(ReplicatedUsing=OnRep_ColorAssigned, VisibleAnywhere, BlueprintReadWrite, Category = Other)
		FLinearColor AssignedColor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

public:
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION()
	void OpenPauseMenu();

	UFUNCTION(Server, WithValidation, unreliable, BlueprintCallable)
	void Server_CreateChatDisplay(const FText &PlayerName, const FText &Message, FLinearColor NewAssignedColor);

	UFUNCTION(NetMulticast, unreliable, BlueprintCallable)
	void Multicast_CreateChatDisplay(const FText &PlayerName, const FText &Message, FLinearColor NewAssignedColor);

	UFUNCTION()
		virtual void OnRep_ColorAssigned();
private:
	TSubclassOf<class UUserWidget> ChatDisplayWidgetClass;
	TSubclassOf<class UUserWidget> ChatDisplayTabClass;
	TSubclassOf<class UUserWidget> ChatTabClass;
	TSubclassOf<class UUserWidget> InGameHUDClass;
	float NextDisplayUpdate;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
