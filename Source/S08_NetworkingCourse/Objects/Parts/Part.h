// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/NetworkUI/Menu.h"
#include "Public/Misc/Optional.h"
#include "Components/SceneComponent.h"
#include "Part.generated.h"

UCLASS(Blueprintable, BlueprintType)
class S08_NETWORKINGCOURSE_API UPart : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPart();

	//UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		//class UStaticMeshComponent *PartStaticMesh;
	//UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		//class USkeletalMeshComponent *PartSkeletalMesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class USceneComponent *Scene;

	UPROPERTY()
		class UMenu *MenuRef;
	UPROPERTY()
		class UStaticPartMesh *StaticMeshRef;
	UPROPERTY()
		class USkeletalPartMesh *SkeletalMeshRef;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) override;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//	FPartStats Details = FPartStats();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Settings) struct FPartStats PartSettings;

	//Functions
	virtual void OnSelected();
	virtual void AllowVariableEdit(FComplexFloat &VariableToEdit);
	virtual void AllowVariableEdit(FComplexInt32 &VariableToEdit);
	virtual void AllowVariableEdit(FComplexString &VariableToEdit);
};
