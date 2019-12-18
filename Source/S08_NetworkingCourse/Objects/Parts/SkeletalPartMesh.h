// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "SkeletalPartMesh.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API USkeletalPartMesh : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	class UPart *PartRef;
};
