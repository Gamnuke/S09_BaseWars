// Fill out your copyright notice in the Description page of Project Settings.


#include "ComplexFloatSetting.h"
#include "BasicTypes.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"

bool UComplexFloatSetting::Initialize() {
	bool Success = Super::Initialize();
	//ProceedButton->OnClicked.AddDynamic(this, &UMenu::PurchaseItem);

	return true;
}
void UComplexFloatSetting::SliderChange(float Value) {
	if (AssignedVariable == nullptr) { return; }
	int32 NewValue = AssignedVariable->Limit.X + (Value*(AssignedVariable->Limit.Y - AssignedVariable->Limit.X));
	Current->SetText(FText::FromString(FString::FromInt(NewValue)));
	AssignedVariable->ChangeValue(NewValue);
}

void UComplexFloatSetting::SetupSetting(FComplexInt32 *ComplexInt)
{
	if (ComplexInt == nullptr) { return; }
	AssignedVariable = ComplexInt;
	Min->SetText(FText::FromString(FString::FromInt(ComplexInt->Limit.X)));
	Max->SetText(FText::FromString(FString::FromInt(ComplexInt->Limit.Y)));
	Current->SetText(FText::FromString(FString::FromInt(ComplexInt->Value)));
	Name->SetText(FText::FromString(ComplexInt->VariableName));

	SliderW->SetValue((ComplexInt->Value - ComplexInt->Limit.X) / (ComplexInt->Limit.Y - ComplexInt->Limit.X));
}
