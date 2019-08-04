// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/InteractableObject.h"
#include "GameFramework/Actor.h"
#include "Browser.generated.h"

UCLASS()
class S08_NETWORKINGCOURSE_API ABrowser : public AInteractableObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABrowser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UWidgetComponent *ScreenUI;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UStaticMeshComponent *TabletBase;

	UFUNCTION(BlueprintNativeEvent)
		void Interacted(class AMainCharacter *Character);

	virtual void Interact(class AMainCharacter *Character);

	UFUNCTION(BlueprintCallable)
		void ExitFocus();

};
