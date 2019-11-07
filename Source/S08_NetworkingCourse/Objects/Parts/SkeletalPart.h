// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "UI/NetworkUI/Menu.h"
#include "Public/Misc/Optional.h"
#include "Components/MeshComponent.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Objects/Parts/CommonPartInterface.h"
#include "SkeletalPart.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class S08_NETWORKINGCOURSE_API USkeletalPart : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	// Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) TEnumAsByte<ESubCategory> Category;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) int32 Cost;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) int32 Health;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bLockedByDefault = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bUsesPhysics = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bUsesSkeletalMesh = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bModifiable = true;

	//Functions
	virtual void OnSelected();
	virtual void AllowVariableEdit(float &VariableToEdit);
	virtual void AllowVariableEdit(int32 &VariableToEdit);
	virtual void AllowVariableEdit(FString &VariableToEdit);
};


