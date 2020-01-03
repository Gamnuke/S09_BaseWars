// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplexFloatSetting.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UComplexFloatSetting : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class USlider *SliderW;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *Min;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *Max;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *Current;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UTextBlock *Name;

	virtual bool Initialize();
	UFUNCTION(BlueprintCallable)
	void SliderChange(float Value);

	struct FComplexInt32 *AssignedVariable = nullptr;
	void SetupSetting(struct FComplexInt32 *ComplexInt);
};
