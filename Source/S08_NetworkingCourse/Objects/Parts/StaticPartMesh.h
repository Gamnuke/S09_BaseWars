// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "StaticPartMesh.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UStaticPartMesh : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY()
		class UPart *PartRef;
};
