// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/NetworkUI/Menu.h"
#include "Public/Misc/Optional.h"
#include "Components/SceneComponent.h"
#include "BasicTypes.h"
#include "Part.generated.h"

UCLASS(Blueprintable, BlueprintType)
class S08_NETWORKINGCOURSE_API UPart : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPart();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UStaticMeshComponent *PartStaticMesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USkeletalMeshComponent *PartSkeletalMesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USceneComponent *Scene;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) override;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//	FPartStats Details = FPartStats();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Settings) FPartStats PartSettings = FPartStats();

	//Functions
	virtual void OnSelected();
	virtual void AllowVariableEdit(float &VariableToEdit, FVector2D Limits);
	virtual void AllowVariableEdit(int32 &VariableToEdit, FVector2D Limits);
	virtual void AllowVariableEdit(FString &VariableToEdit, int32 Limits);
};
