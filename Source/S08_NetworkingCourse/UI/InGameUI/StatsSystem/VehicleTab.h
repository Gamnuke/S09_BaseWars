// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VehicleTab.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UVehicleTab : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UEditableTextBox *VehicleName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UButton * SaveVehicleButton;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UButton * LoadVehicleButton;

	FString FilePath;
	class UMenu * ParentMenu;
	
protected: 
	UFUNCTION()
		void SaveVehicle();

	UFUNCTION()
		void LoadVehicle();

};
