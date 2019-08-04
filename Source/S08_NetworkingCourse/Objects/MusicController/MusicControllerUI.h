// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MusicControllerUI.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UMusicControllerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
		class UButton *RefreshButton;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
		class UButton *SelectButton;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
		class UButton *ExitButton;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
		class UScrollBox *MusicScrollBox;

	class AMusicController *OwningController;

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void RefreshMusic();

	UFUNCTION()
	void SelectMusic();

	UFUNCTION()
		void Exit();
public:
	TArray<class USoundWave*> GetSoundWaveFromFile();
	TArray<FString> MusicNames;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<class UMusicTab> MusicTabClass;
};
