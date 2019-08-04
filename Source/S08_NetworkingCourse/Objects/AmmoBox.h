// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/InteractableObject.h"
#include "AmmoBox.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API AAmmoBox : public AInteractableObject
{
	GENERATED_BODY()
AAmmoBox();

protected:

	virtual void Interact(AMainCharacter * Character);

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UStaticMeshComponent *Mesh;

};
