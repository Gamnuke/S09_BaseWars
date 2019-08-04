// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnrealNetwork.h"
#include "InGameHUD.generated.h"

class UVerticalBox;
class UScrollBox;
class AGamePlayerController;

UCLASS()
class S08_NETWORKINGCOURSE_API UInGameHUD : public UUserWidget
{
	GENERATED_BODY()
protected:
	UInGameHUD(const FObjectInitializer& ObjectInitializer);
	virtual bool Initialize();
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	
private:
	float TargetOpacity;

public:
	UFUNCTION(BlueprintCallable)
		void ComposeNewMessage(FText NewPlayerName, FText NewMessage);
	UFUNCTION(BlueprintCallable)
		void FormatChatBox();
	UFUNCTION()
		void OnTextCommitted(const FText & Text, ETextCommit::Type CommitType);

	TSubclassOf<class UUserWidget> ChatTabClass;

	void Setup();

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		UScrollBox *ChatBox;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UEditableText *TextInput;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UBorder *FadeBorder;

	//Crosshair
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UImage *LeftHair;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UImage *RightHair;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UImage *TopHair;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UImage *BottomHair;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UImage *MiddleHair;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UImage *Hitmarker;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UCircularThrobber *ReloadThrobber;

	// Bottom Display
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UTextBlock *AmmoDisplay;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UProgressBar *HealthBar;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UImage *DamageVignette;

	// Top display
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UTextBlock *RoundDisplay;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UProgressBar *RoundProgress;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, VisibleAnywhere)
		class UTextBlock *PointsDisplay;

	FVector ProjectedLocation;
	FVector ProjectedDirection;

	float VignetteTargetOpacity = 0;

	AGamePlayerController *Owner;
};
