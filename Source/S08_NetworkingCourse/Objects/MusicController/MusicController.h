// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/InteractableObject.h"
#include "MusicController.generated.h"

UCLASS()
class S08_NETWORKINGCOURSE_API AMusicController : public AInteractableObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicController();

	class USceneComponent *Root;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UWidgetComponent *Display;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UStaticMeshComponent *Base;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UMusicControllerUI *DisplayUI;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UAudioComponent *AudioComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AMainCharacter * Character);

	USoundWave *SelectedMusic;
	FString SelectedMusicDir;

	void SetWaveFile(USoundWave *Music);

	UFUNCTION()
	void OnFinishedGenerate();

	UFUNCTION()
	void PlayMusic();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UMediaSoundComponent *MediaSoundComponent;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		void OnPlayMusicDelay();

	UFUNCTION(BlueprintCallable, Category = "Character")
		void OnAfterPlayMusicDelayed();


};
